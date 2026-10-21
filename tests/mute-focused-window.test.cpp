// Unit tests for the mod's pure logic. tools/verify.ps1 builds this with the
// Windhawk editor's no-op API stubs (WH_EDITING) and runs it; a nonzero exit
// code fails the gate.
#include "../mods/mute-focused-window.wh.cpp"

#include <cstdio>

int g_failures;

void Check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::fprintf(stderr, "line %d: CHECK(%s) failed\n", line, expression);
        g_failures++;
    }
}

#define CHECK(condition) Check((condition), #condition, __LINE__)

bool ParsesTo(std::wstring_view text, UINT modifiers, UINT virtualKey) {
    std::optional<Hotkey> hotkey = ParseHotkey(text);
    return hotkey && hotkey->modifiers == modifiers &&
           hotkey->virtualKey == virtualKey;
}

void TestParseHotkey() {
    CHECK(ParsesTo(L"Ctrl+Alt+M", MOD_CONTROL | MOD_ALT, 'M'));
    CHECK(ParsesTo(L" ctrl + shift + f9 ", MOD_CONTROL | MOD_SHIFT, VK_F9));
    CHECK(ParsesTo(L"Control+Windows+Delete", MOD_CONTROL | MOD_WIN,
                   VK_DELETE));
    CHECK(ParsesTo(L"Win+m", MOD_WIN, 'M'));
    CHECK(ParsesTo(L"Alt+7", MOD_ALT, '7'));
    CHECK(ParsesTo(L"Pause", 0, VK_PAUSE));
    CHECK(ParsesTo(L"F1", 0, VK_F1));
    CHECK(ParsesTo(L"F24", 0, VK_F24));
    CHECK(ParsesTo(L"Shift+Numpad5", MOD_SHIFT, VK_NUMPAD5));
    CHECK(ParsesTo(L"Ctrl+VolumeMute", MOD_CONTROL, VK_VOLUME_MUTE));
    CHECK(ParsesTo(L"Win+0x4D", MOD_WIN, 0x4D));
    CHECK(ParsesTo(L"Win+0xfe", MOD_WIN, 0xFE));
    // Repeating a modifier is harmless.
    CHECK(ParsesTo(L"Ctrl+Ctrl+M", MOD_CONTROL, 'M'));

    CHECK(!ParseHotkey(L""));
    CHECK(!ParseHotkey(L"   "));
    CHECK(!ParseHotkey(L"Ctrl+Alt"));
    CHECK(!ParseHotkey(L"Ctrl++M"));
    CHECK(!ParseHotkey(L"Ctrl+M+"));
    CHECK(!ParseHotkey(L"Ctrl+M+N"));
    CHECK(!ParseHotkey(L"Hyper+M"));
    CHECK(!ParseHotkey(L"F0"));
    CHECK(!ParseHotkey(L"F01"));
    CHECK(!ParseHotkey(L"F25"));
    CHECK(!ParseHotkey(L"0x0"));
    CHECK(!ParseHotkey(L"0xFF"));
    CHECK(!ParseHotkey(L"0x"));
    CHECK(!ParseHotkey(L"0xG1"));
    CHECK(!ParseHotkey(L"Numpad10"));
    CHECK(!ParseHotkey(L"!"));
}

void TestCollectProcessFamily() {
    // Root 10 starts at time 100.
    const std::vector<ProcessInfo> processes = {
        {10, 1, 100},
        {11, 10, 110},  // Windowless child, such as a browser's audio process.
        {12, 11, 120},  // Grandchild.
        {13, 10, 50},   // Started before 10, so its parent ID was reused.
        {14, 10, 130},  // Owns a window: a separate app.
        {15, 14, 140},  // Child of the separate app.
        {16, 99, 150},  // Unrelated.
        {17, 10, 0},    // Can't be opened, so the link can't be confirmed.
        {0, 0, 0},      // The idle process lists itself as its parent.
    };
    const std::unordered_set<DWORD> windowOwners = {10, 14};

    CHECK((CollectProcessFamily(10, processes, windowOwners) ==
           std::unordered_set<DWORD>{10, 11, 12}));
    // A root whose creation time is unknown keeps only itself.
    CHECK((CollectProcessFamily(17, processes, windowOwners) ==
           std::unordered_set<DWORD>{17}));
    // A root that has already exited keeps only itself.
    CHECK((CollectProcessFamily(42, processes, windowOwners) ==
           std::unordered_set<DWORD>{42}));
    CHECK((CollectProcessFamily(0, processes, windowOwners) ==
           std::unordered_set<DWORD>{0}));
}

// A session's volume control whose reads and writes can be made to fail. The
// test owns it on the stack; reference counting never deletes it.
class FakeVolume : public ISimpleAudioVolume {
   public:
    FakeVolume(BOOL muted, HRESULT getResult = S_OK, HRESULT setResult = S_OK)
        : muted(muted), getResult(getResult), setResult(setResult) {}

    BOOL muted;
    HRESULT getResult;
    HRESULT setResult;
    int setCalls = 0;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void** object) override {
        *object = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE SetMasterVolume(float, LPCGUID) override {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE GetMasterVolume(float*) override {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE SetMute(const BOOL mute, LPCGUID) override {
        setCalls++;
        if (SUCCEEDED(setResult)) {
            muted = mute;
        }
        return setResult;
    }
    HRESULT STDMETHODCALLTYPE GetMute(BOOL* mute) override {
        if (SUCCEEDED(getResult)) {
            *mute = muted;
        }
        return getResult;
    }
};

std::vector<ComPtr<ISimpleAudioVolume>> Sessions(
    std::initializer_list<FakeVolume*> fakes) {
    std::vector<ComPtr<ISimpleAudioVolume>> volumes;
    for (FakeVolume* fake : fakes) {
        volumes.emplace_back(fake);
    }
    return volumes;
}

void TestToggleSessions() {
    {
        // A muted session plus one on a device that was just unplugged: the
        // press unmutes, rather than the unreadable session counting as
        // unmuted and turning it into a mute.
        FakeVolume healthy(TRUE);
        FakeVolume unplugged(FALSE, AUDCLNT_E_DEVICE_INVALIDATED,
                             AUDCLNT_E_DEVICE_INVALIDATED);
        SessionToggle toggle = ToggleSessions(Sessions({&healthy, &unplugged}));
        CHECK(toggle.result == ToggleResult::kToggled);
        CHECK(!toggle.mute);
        CHECK(!healthy.muted);
        CHECK(unplugged.setCalls == 0);
        CHECK(toggle.changed == 1 && toggle.failed == 1);
    }
    {
        // A partly muted app becomes fully muted.
        FakeVolume muted(TRUE), playing(FALSE);
        SessionToggle toggle = ToggleSessions(Sessions({&muted, &playing}));
        CHECK(toggle.result == ToggleResult::kToggled && toggle.mute);
        CHECK(muted.muted && playing.muted);
        CHECK(toggle.changed == 2 && toggle.failed == 0);
    }
    {
        // A fully muted app is unmuted.
        FakeVolume first(TRUE), second(TRUE);
        SessionToggle toggle = ToggleSessions(Sessions({&first, &second}));
        CHECK(toggle.result == ToggleResult::kToggled && !toggle.mute);
        CHECK(!first.muted && !second.muted);
    }
    {
        // One failed write still toggles the rest and is counted.
        FakeVolume works(FALSE), rejects(FALSE, S_OK, E_FAIL);
        SessionToggle toggle = ToggleSessions(Sessions({&works, &rejects}));
        CHECK(toggle.result == ToggleResult::kToggled && toggle.mute);
        CHECK(works.muted && !rejects.muted);
        CHECK(toggle.changed == 1 && toggle.failed == 1);
    }
    {
        // Every write failing is a failure, not a toggle.
        FakeVolume first(FALSE, S_OK, E_FAIL), second(FALSE, S_OK, E_FAIL);
        SessionToggle toggle = ToggleSessions(Sessions({&first, &second}));
        CHECK(toggle.result == ToggleResult::kFailed);
        CHECK(toggle.changed == 0 && toggle.failed == 2);
    }
    {
        // Every read failing changes nothing.
        FakeVolume unreadable(TRUE, E_FAIL);
        SessionToggle toggle = ToggleSessions(Sessions({&unreadable}));
        CHECK(toggle.result == ToggleResult::kFailed);
        CHECK(unreadable.setCalls == 0 && unreadable.muted);
    }
}

void TestFallback() {
    CHECK(ShouldFallBackToSystemMute(ToggleResult::kNoAppAudio, true));
    CHECK(!ShouldFallBackToSystemMute(ToggleResult::kNoAppAudio, false));
    // A failure may leave the app playing; muting everything would hit
    // unrelated apps.
    CHECK(!ShouldFallBackToSystemMute(ToggleResult::kFailed, true));
    CHECK(!ShouldFallBackToSystemMute(ToggleResult::kToggled, true));

    // An incomplete search that found nothing is a failure, so it can't
    // trigger the fallback either.
    CHECK(ResultForNoSessions(true) == ToggleResult::kNoAppAudio);
    CHECK(ResultForNoSessions(false) == ToggleResult::kFailed);
    CHECK(!ShouldFallBackToSystemMute(ResultForNoSessions(false), true));
}

int main() {
    TestParseHotkey();
    TestCollectProcessFamily();
    TestToggleSessions();
    TestFallback();
    if (g_failures) {
        std::fprintf(stderr, "%d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("All checks passed\n");
    return 0;
}
