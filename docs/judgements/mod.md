# Mod judgements

Human checks of the mod running in Windhawk. Intended behaviour is in [[design/mod|Mod behaviour]].

### Hotkeys toggle the focused app's mute
*Judged 2026-10-21, on Windows 11 with a Logitech G915. Cicely tried Ctrl+Alt+M, Ctrl+VolumeMute and VolumeMute as the hotkey in Windhawk 1.7.3 and reported that each works.*

**Exercised:** the mod, pasted into the Windhawk 1.7.3 editor and compiled as `local@mute-focused-window`. It ran in its own `windhawk.exe -tool-mod` process, with the default hotkey and then each Mute-key setting in turn. The G915's dedicated Mute key provided `VolumeMute`.

**Verdict:** "Works great" with all three hotkeys. The Mute key works as a hotkey on this keyboard, with no vendor-specific handling.

**Not covered by this report:** which apps were tried, including whether a Chromium browser, an Electron app and a Store app were among them. Also not covered: two apps playing at once, presses on the desktop, taskbar or File Explorer, the **Mute the system when there's no app to mute** fallback, invalid hotkey logging, and the default hotkey's known conflicts. The remaining steps are listed in [Issue #1](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues/1). They were covered later the same day: see [[judgements/mod#App matching, Explorer, logging and fallback work|App matching, Explorer, logging and fallback work]] and [[judgements/mod#Ctrl+Alt+M is an acceptable default|Ctrl+Alt+M is an acceptable default]].

**Recheck when:** hotkey registration, process-family matching or toggling changes, when moving to Windhawk 2.0, or when keyboard software reassigns the Mute key.

**Status:** made on builds before commit `7cd5127`. That commit changed toggling, logging and the fallback, which met this entry's recheck condition, so the verdict was provisional for that build. Cicely rechecked it the same day: see [[judgements/mod#Revised build passes the earlier checks|Revised build passes the earlier checks]].

Evidence: [Issue #1](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues/1), [PR #3](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/pull/3).

### App matching, Explorer, logging and fallback work
*Judged 2026-10-21, on Windows 11 with a Logitech G915. Cicely confirmed that browser, Electron and Store apps mute, that muting one of two playing apps leaves the other playing, that presses on the desktop, taskbar and File Explorer do nothing, that an invalid hotkey is logged and that the system mute fallback works.*

**Exercised:** the remaining checklist of [Issue #1](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues/1), in Windhawk 1.7.3, with the same build as [[judgements/mod#Hotkeys toggle the focused app's mute|Hotkeys toggle the focused app's mute]].

**Verdict:** all of these worked as described in [[design/mod|Mod behaviour]]. That includes process-family matching for apps that play audio from a child process, the Explorer exclusion, logging of an invalid `hotkey` setting, and **Mute the system when there's no app to mute**.

**Not covered:** which browser, Electron app and Store app were used was not recorded. Several output devices at once and elevated (administrator) apps were not part of the checklist.

**Recheck when:** process-family matching, the Explorer exclusion, hotkey parsing or logging, or the fallback changes, or when moving to Windhawk 2.0.

**Status:** made on the build at commit `0849c50`. Commit `7cd5127` changed toggling, logging and the fallback, which met this entry's recheck condition, so the verdict was provisional for that build. Cicely rechecked it the same day: see [[judgements/mod#Revised build passes the earlier checks|Revised build passes the earlier checks]].

### Ctrl+Alt+M is an acceptable default
*Judged 2026-10-21, on Windows 11 with a Logitech G915. Cicely accepted Ctrl+Alt+M as the default hotkey.*

**Context:** the known conflicts are recorded in [[decisions/mod#Default hotkey Ctrl+Alt+M|Default hotkey Ctrl+Alt+M]]: AltGr+M on some European layouts, and IntelliJ's Extract Method. This verdict accepts them for the default; the setting lets anyone choose another combination.

**Recheck when:** the default changes, or reports show the conflicts matter more than expected.

### Revised build passes the earlier checks
*Judged 2026-10-21, on Windows 11 with a Logitech G915. Cicely repeated the earlier hands-on checks on the build at commit 7cd5127 in Windhawk 1.7.3 and reported that they pass.*

**Exercised:** the build that keeps failed audio operations out of the toggle ([Issue #4](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues/4)). Before the check, Windhawk's editor copy was confirmed to match commit `7cd5127`, and its tool process had restarted with the new build. The earlier checks are those of [[judgements/mod#Hotkeys toggle the focused app's mute|Hotkeys toggle the focused app's mute]] and [[judgements/mod#App matching, Explorer, logging and fallback work|App matching, Explorer, logging and fallback work]]:
- the three hotkeys;
- browser, Electron and Store apps;
- muting one of two playing apps;
- the desktop, the taskbar and File Explorer;
- invalid hotkey logging;
- the system mute fallback.

**Verdict:** they pass on this build, so both earlier verdicts hold for it.

**Not covered:** the wider check in [Issue #5](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues/5): several output devices at once, elevated apps, recording which apps were used, and confirming that the Store app used the framed-UWP path. The audio failure cases themselves are covered by unit tests, not by this check.

**Recheck when:** hotkey registration, process-family matching, toggling, logging or the fallback changes, or when moving to Windhawk 2.0.
