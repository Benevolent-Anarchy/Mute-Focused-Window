# Mod design decisions

Current behaviour is in [[design/mod|Mod behaviour]]. How the mod is checked is in [[decisions/verification|Verification decisions]].

## 2026-10-21

### Run as a Windhawk tool mod

The mod runs in its own `windhawk.exe` process using the tool-mod snippet from the Windhawk wiki page [Mods as tools](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process), pasted unchanged.

It needs only a global hotkey and the Core Audio session API, and neither requires running inside another process. The wiki recommends tool mods for this case. Injecting into `explorer.exe` was rejected: there can be several Explorer processes, a bug would affect the whole shell, and nothing ties the feature to Explorer. Injecting into every app, as the existing `mute-in-background` mod does, was rejected because it needs per-app setup and still misses audio played by child processes.

The Windhawk 2.0 alpha header `windhawk_tool_mod.h` would replace the snippet but doesn't compile on the current release, 1.7.3. The wiki says 2.0 keeps running snippet-based mods.

### Match the focused app by its process family

The focused app's audio is the sessions of its process and of its descendants. The search stops at any descendant that owns its own top-level window, and parent links are only trusted when the child started after the parent.

Matching only the window's own process was rejected. Chromium browsers, Electron apps such as Discord, and WebView2 apps play audio from a child process, so the hotkey would do nothing for them. Matching every process with the same executable was rejected because it would also mute other running copies of the same program. Reading child command lines to find Chromium's audio service, as the `taskbar-volume-control-per-app` mod does, was rejected: it needs memory access to other processes, fails on elevated apps and depends on Chromium's command-line flags.

Evidence: a read-only run on Windows 11 with this matching found the sessions of Chrome, Brave, Discord, WhatsApp, Plex and DuckDuckGo, each reached through the app's window-owning process.

### Toggle all of an app's sessions together

One press mutes every matched session unless all of them are already muted, in which case it unmutes them all. Flipping each session independently was rejected because an app left partly muted, for example by the Volume Mixer, would stay split after every press.

### Do nothing for Explorer

The mod ignores a focused window owned by Explorer, matched by the shell's process ID or by its executable path.

Explorer starts nearly every app, so its windowless descendants include background apps that have nothing to do with the focused window. The read-only run found five such sessions under Explorer. Comparing executable paths also covers folder windows running in a separate `explorer.exe` process. Matching only Explorer's own process, without descendants, was rejected as unnecessary: the shell plays no app audio of its own outside the system sounds session.

### Default hotkey Ctrl+Alt+M

The default hotkey is `Ctrl+Alt+M`, chosen as M for mute. The setting accepts any combination.

Known conflicts: on keyboard layouts with AltGr, AltGr acts as Ctrl+Alt, so AltGr+M (µ on German and Nordic layouts) triggers the hotkey instead of typing. IntelliJ-based editors use `Ctrl+Alt+M` for Extract Method. Rejected defaults: `Ctrl+Shift+M` is mute in Teams and Discord, `Win+Ctrl+M` opens Magnifier settings, `Win+Shift+M` restores minimised windows, and `Win+Alt+M` belongs to the Xbox Game Bar.

### Read the hotkey as text

The `hotkey` setting is a `Modifier+Key` string, parsed by the mod. Windhawk 1.7.3 has no hotkey setting type; the `$format: hotkey` annotation exists only in the 2.0 alpha. Separate key and modifier checkbox settings, as in `force-kill-active-window`, were rejected as wordier for the same result.
