# Mute Focused Window

A Windhawk mod that mutes or unmutes the focused window with a hotkey.

Start with [the project map](docs/NEXT.md). [Design](docs/DESIGN.md), [decisions](docs/DECISIONS.md) and [human judgements](docs/JUDGEMENTS.md) form the permanent canon. Unfinished work and questions belong in [GitHub Issues](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues).

[AGENTS.md](AGENTS.md) defines the shared working process for every agent.

## Documentation foundation setup

This preset supplies documentation and the working process. It has no engine dependencies. Use PowerShell 7.2 or newer and Git.

Before opening a PR, run the complete gate from PowerShell 7.2 or newer:

```powershell
pwsh -NoProfile -File tools/verify.ps1
```

The complete initial gate validates documentation. Extend this same gate as project checks are added.

Put exports in `dist/`, which is ignored at every depth. Local Obsidian settings and plugins live in the ignored `docs/.obsidian/`; apply the shared `ba-bootstrap` skill's local-setup mode after a fresh clone. The native project gate runs from this checkout without an installed skill.
