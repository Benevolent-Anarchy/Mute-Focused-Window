# Mute Focused Window

A Windhawk mod that mutes or unmutes the focused window with a hotkey.

Start with [the project map](docs/NEXT.md). [Design](docs/DESIGN.md), [decisions](docs/DECISIONS.md) and [human judgements](docs/JUDGEMENTS.md) form the permanent canon. Unfinished work and questions belong in [GitHub Issues](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues).

[AGENTS.md](AGENTS.md) defines the shared working process for every agent.

## The mod

The whole mod is [mods/mute-focused-window.wh.cpp](mods/mute-focused-window.wh.cpp), released under the [MIT License](mods/LICENSE). Its readme block, near the top of the file, explains what it does and how to change the hotkey; [the design note](docs/design/mod.md) gives the detail.

Install it from Windhawk's catalogue: [windhawk.net/mods/mute-focused-window](https://windhawk.net/mods/mute-focused-window). [The publishing note](docs/design/publishing.md) explains how a new version ships there.

To try a development version instead, open Windhawk 1.7.3 and choose **Create a New Mod**. Replace the template with the file's contents and press **Compile Mod** (Ctrl+B). The default hotkey is Ctrl+Alt+M. **Enable logging** and **Show Log Output** in the editor's sidebar show what each press did.

## Documentation foundation setup

This preset supplies documentation and the working process. Use PowerShell 7.2 or newer and Git. The gate also needs [Windhawk](https://windhawk.net/) installed, because it builds the mod with Windhawk's own compiler. For a portable Windhawk, set `WINDHAWK_ROOT` to its folder.

Before opening a PR, run the complete gate from PowerShell 7.2 or newer:

```powershell
pwsh -NoProfile -File tools/verify.ps1
```

The gate validates documentation, checks the mod's metadata, builds the mod for every architecture Windhawk would, and runs the unit tests in `tests/`. Build output goes to the ignored `.test-results/`. Extend this same gate as project checks are added.

Put exports in `dist/`, which is ignored at every depth. Local Obsidian settings and plugins live in the ignored `docs/.obsidian/`; apply the shared `ba-bootstrap` skill's local-setup mode after a fresh clone. The native project gate runs from this checkout without an installed skill.
