# Publishing decisions

The current process is in [[design/publishing|Publishing]]. The `@github` choice that shapes it is [[decisions/mod#Credit Benevolent Anarchy with a personal GitHub link|Credit Benevolent Anarchy with a personal GitHub link]].

## 2026-10-21

### Publish from main through a personal fork

The published file is copied, unchanged, from `main` here into a branch of the fork `cicely-f/windhawk-mods`. The PR to `ramensoftware/windhawk-mods` is opened from that account.

windhawk-mods requires `@github` to be the account opening the PR, and an organisation can't open one, so the fork belongs to Cicely's personal account. Copying from merged `main` means every published version has passed this project's gate, unit tests and review, and the docs describe exactly what users run.

Rejected: maintaining the mod directly in the fork. That would lose the gate, tests and canon here. Also rejected: syncing from an unmerged branch. During the 1.0 review, #6456 briefly carried PR #7's version before #7 merged; it matched once #7 merged, but syncing only from merged `main` avoids that window.

Evidence: version 1.0 was accepted on 2026-10-22, merged by m417z, and the published file is byte-identical to `main` at `3e57860`.

### Batch small fixes into the next catalogue update

While a windhawk-mods PR waits for a human maintainer, small non-blocking fixes wait for the next update instead of being pushed to that PR. They go out together with any changes the maintainer asks for, or in the next version after acceptance.

Any push takes the PR out of the maintainers' queue and needs a new `/ai-review` and `/ready-for-reviewer`, so pushing a rare edge-case fix costs more than the fix is worth. Cicely chose this for [Issue #8](https://github.com/Benevolent-Anarchy/Mute-Focused-Window/issues/8), which is planned as version 1.0.1.

Rejected: pushing every fix to the waiting PR as soon as it's ready.
