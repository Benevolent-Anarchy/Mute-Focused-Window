# Mute Focused Window — Design

This indexes the current intended design in topical notes under `design/`. Add actual design as it is settled; the project description is not a speculative design brief.

## Where information belongs

Current intended behaviour belongs here. Reasoning and rejected alternatives belong in [[DECISIONS]], human observations in [[JUDGEMENTS]], and orientation in [[NEXT]]. Unresolved work, questions and discussions belong in [Issues](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues).

## Authoring a topic

Give each topical note one clear topic, a `#` title and searchable headings. Its index group links the actual file and uses its title; beneath it link every substantive `##`–`######` heading and add useful search terms. Keep the index a reading guide rather than copying the design. Remove that directory's `.gitkeep` when adding its first note.

Wikilinks resolve from the `docs/` vault root with `.md` implicit; heading targets are exact and unambiguous. Markdown links resolve relative to the note. The complete `tools/verify.ps1` gate checks navigation and agreement in both directions.

```markdown
### Interface — [[design/interface|Interface]]
- [[design/interface#Keyboard navigation|Keyboard navigation]] — keyboard, focus
```

## Index

### Mod behaviour — [[design/mod|Mod behaviour]]
- [[design/mod#Hotkey|Hotkey]] — setting format, modifiers, key names, MOD_NOREPEAT, registration failure, log
- [[design/mod#Which audio belongs to the focused app|Which audio belongs to the focused app]] — foreground window, UWP, ApplicationFrameHost, child processes, process tree, browsers, Electron, PID reuse, system sounds, Explorer
- [[design/mod#Toggling|Toggling]] — mute, unmute, all sessions, Volume Mixer, persistence
- [[design/mod#Process and lifetime|Process and lifetime]] — tool mod, windhawk.exe, thread, message loop, COM, settings change, unload
