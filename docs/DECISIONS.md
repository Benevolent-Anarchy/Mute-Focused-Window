# Mute Focused Window — Decisions

This indexes significant reasoning in topical files under `decisions/`, so related choices are encountered together. Current intended behaviour belongs in [[DESIGN]], checks that required a person in [[JUDGEMENTS]], orientation in [[NEXT]], and unresolved matters in [Issues](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues).

## Authoring a record

Use real dated `## YYYY-MM-DD` groups and a unique `### Decision title` for each entry. Titles are durable citation keys. Record the choice, why it was made, rejected alternatives, evidence and any superseded record. Mark superseded entries in place and link forward; preserve history and say what survives. Update current design in the same change when intended behaviour changes.

Group indexed entries beneath a heading linking their actual topic file. Link every title to its heading and add terms a reader might search for, including terms absent from the title. Cross-link related topics rather than duplicating a record. Wikilinks are relative to the `docs/` vault root; Markdown links are relative to the note. Remove `.gitkeep` when adding the first topic. `tools/verify.ps1` checks the file/title relationship and unique citation keys.

```markdown
### Interface — [[decisions/interface|Interface]]
- [[decisions/interface#Keyboard navigation|Keyboard navigation]] — keyboard, focus
```

## Index

### Mod design — [[decisions/mod|Mod design decisions]]
- [[decisions/mod#Run as a Windhawk tool mod|Run as a Windhawk tool mod]] — tool mod, dedicated process, explorer.exe injection, mute-in-background, Windhawk 2.0, windhawk_tool_mod.h
- [[decisions/mod#Match the focused app by its process family|Match the focused app by its process family]] — child processes, Chromium, Electron, WebView2, process tree, PID reuse, command line
- [[decisions/mod#Toggle all of an app's sessions together|Toggle all of an app's sessions together]] — partly muted, Volume Mixer, sessions
- [[decisions/mod#Do nothing for Explorer|Do nothing for Explorer]] — shell, desktop, taskbar, folder windows, background apps
- [[decisions/mod#Default hotkey Ctrl+Alt+M|Default hotkey Ctrl+Alt+M]] — shortcut, conflicts, AltGr, IntelliJ, Teams, Discord, Game Bar
- [[decisions/mod#Offer a system mute fallback for the Mute key|Offer a system mute fallback for the Mute key]] — VolumeMute, Logitech G915, keyboard, media key, systemMuteFallback, volume indicator, WM_APPCOMMAND
- [[decisions/mod#Keep failed audio operations out of the toggle|Keep failed audio operations out of the toggle]] — GetMute, SetMute, HRESULT, unplugged device, AUDCLNT_E_DEVICE_INVALIDATED, review, fallback, failure
- [[decisions/mod#License the mod under MIT|License the mod under MIT]] — licence, license, @license, copyright, LICENSE
- [[decisions/mod#Credit Benevolent Anarchy with a personal GitHub link|Credit Benevolent Anarchy with a personal GitHub link]] — @author, @github, @homepage, windhawk-mods validator, organisation
- [[decisions/mod#Read the hotkey as text|Read the hotkey as text]] — settings, $format hotkey, checkboxes

### Verification — [[decisions/verification|Verification decisions]]
- [[decisions/verification#Build mods with the installed Windhawk compiler|Build mods with the installed Windhawk compiler]] — gate, clang, llvm-mingw, x86, x86-64, -Werror, WINDHAWK_ROOT, windhawk.ini
- [[decisions/verification#Unit test pure logic by including the mod|Unit test pure logic by including the mod]] — tests, WH_EDITING, stubs, human check
- [[decisions/verification#Check mod metadata in the gate|Check mod metadata in the gate]] — @id, @version, readme, settings, windhawk-mods validator
