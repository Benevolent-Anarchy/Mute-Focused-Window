# Mute Focused Window — Judgements

This indexes human checks and the enduring properties later changes must hold up against, in topical notes under `judgements/`. Examples include appearance, interaction, perceived timing, clarity and difficulty. An agent's inspection or automated test is not evidence of a human check. See [[DESIGN]], [[DECISIONS]] and [[NEXT]]; open checks belong in [Issues](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues).

## Authoring a record

Use a unique `### Judgement title`, immediately followed by its only single-line `*Judged ... .*` statement. Include a real standalone `YYYY-MM-DD` date and independently name applicable hardware as `on <hardware>` at the beginning or after the date in the first sentence. Where hardware cannot affect the observation, explicitly use `Hardware not applicable`. Do not invent a missing date, device or verdict.

Record what was exercised, the actual verdict and accepted properties, what was not covered, what requires a recheck, and evidence/Issue links. Index each title under a heading linking its actual topic file, with the exact Judged statement copied without italics/final full stop, followed by search terms. Wikilinks resolve from `docs/`; Markdown links resolve from the note. Remove `.gitkeep` when adding the first topic.

```markdown
### Interface — [[judgements/interface|Interface]]
- [[judgements/interface#Focus is visible|Focus is visible]] — Judged 2026-10-09, on Windows desktop — focus, contrast
```

For incomplete history only, use `Date not recorded` or `Hardware not recorded` and label both the entry and its index line `Incomplete historical record`. Such a record is not trusted acceptance; open an investigation Issue where needed. These markers never substitute for fields in a new check.

When a change calls a judgement into question, retain and mark the entry provisional and link an investigation Issue. Close that Issue after a new human check is recorded, not merely after explaining the suspected cause. Preserve old and later evidence. The machine gate validates metadata/index agreement and never supplies the human verdict.

## Index

### Mod — [[judgements/mod|Mod judgements]]
- [[judgements/mod#Hotkeys toggle the focused app's mute|Hotkeys toggle the focused app's mute]] — Judged 2026-10-21, on Windows 11 with a Logitech G915. Cicely tried Ctrl+Alt+M, Ctrl+VolumeMute and VolumeMute as the hotkey in Windhawk 1.7.3 and reported that each works — hotkey, Mute key, VolumeMute, G915, Windhawk 1.7.3
- [[judgements/mod#App matching, Explorer, logging and fallback work|App matching, Explorer, logging and fallback work]] — Judged 2026-10-21, on Windows 11 with a Logitech G915. Cicely confirmed that browser, Electron and Store apps mute, that muting one of two playing apps leaves the other playing, that presses on the desktop, taskbar and File Explorer do nothing, that an invalid hotkey is logged and that the system mute fallback works — child processes, UWP, Explorer exclusion, log, systemMuteFallback
- [[judgements/mod#Ctrl+Alt+M is an acceptable default|Ctrl+Alt+M is an acceptable default]] — Judged 2026-10-21, on Windows 11 with a Logitech G915. Cicely accepted Ctrl+Alt+M as the default hotkey — default hotkey, AltGr, IntelliJ, conflicts
