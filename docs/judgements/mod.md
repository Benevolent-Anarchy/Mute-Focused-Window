# Mod judgements

Human checks of the mod running in Windhawk. Intended behaviour is in [[design/mod|Mod behaviour]].

### Hotkeys toggle the focused app's mute
*Judged 2026-10-21, on Windows 11 with a Logitech G915. Cicely tried Ctrl+Alt+M, Ctrl+VolumeMute and VolumeMute as the hotkey in Windhawk 1.7.3 and reported that each works.*

**Exercised:** the mod, pasted into the Windhawk 1.7.3 editor and compiled as `local@mute-focused-window`. It ran in its own `windhawk.exe -tool-mod` process, with the default hotkey and then each Mute-key setting in turn. The G915's dedicated Mute key provided `VolumeMute`.

**Verdict:** "Works great" with all three hotkeys. The Mute key works as a hotkey on this keyboard, with no vendor-specific handling.

**Not covered by this report:** which apps were tried, including whether a Chromium browser, an Electron app and a Store app were among them. Also not covered: two apps playing at once, presses on the desktop, taskbar or File Explorer, the **Mute the system when there's no app to mute** fallback, invalid hotkey logging, and the default hotkey's known conflicts. The remaining steps are listed in [Issue #1](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues/1).

**Recheck when:** hotkey registration, process-family matching or toggling changes, when moving to Windhawk 2.0, or when keyboard software reassigns the Mute key.

Evidence: [Issue #1](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues/1), [PR #3](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/pull/3).
