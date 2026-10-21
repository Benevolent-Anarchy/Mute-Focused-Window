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

void TestShouldMute() {
    CHECK(!ShouldMute({}));
    CHECK(ShouldMute({FALSE}));
    CHECK(!ShouldMute({TRUE}));
    CHECK(ShouldMute({TRUE, FALSE}));
    CHECK(!ShouldMute({TRUE, TRUE}));
}

int main() {
    TestParseHotkey();
    TestCollectProcessFamily();
    TestShouldMute();
    if (g_failures) {
        std::fprintf(stderr, "%d check(s) failed\n", g_failures);
        return 1;
    }
    std::printf("All checks passed\n");
    return 0;
}
