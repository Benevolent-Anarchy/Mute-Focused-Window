# Mod behaviour

The mod is a single Windhawk source file, [mute-focused-window.wh.cpp](../../mods/mute-focused-window.wh.cpp), with the Windhawk ID `mute-focused-window`. Its readme block is the user-facing description; this note records the intended behaviour behind it. Reasoning and rejected alternatives are in [[decisions/mod|Mod design decisions]].

## Hotkey

A single global hotkey toggles mute for the focused app. The `hotkey` setting holds any modifiers and exactly one key joined by `+`, case-insensitive, with optional spaces: for example `Ctrl+Alt+M`, the default. Modifiers are `Ctrl`/`Control`, `Alt`, `Shift` and `Win`/`Windows`. Keys are letters, digits, `F1`–`F24`, the named keys listed in the readme, or a hexadecimal virtual-key code from `0x01` to `0xFE`.

The hotkey is registered with `MOD_NOREPEAT`, so holding it toggles once. An invalid setting or a combination already owned by another program leaves no hotkey registered and writes the reason to the mod's log. Changing the setting re-registers the hotkey without restarting the mod.

## Which audio belongs to the focused app

The focused app is the process owning the foreground window. For a UWP window framed by `ApplicationFrameHost.exe`, it is the process owning the framed `Windows.UI.Core.CoreWindow` instead.

The app's audio is every session owned by that process or by its descendants, on every active output device. A descendant that owns its own top-level window (visible, unowned and not a tool window) is a separate app: it and its own descendants are excluded. A process counts as a descendant only if it started after its recorded parent, because Windows reuses process IDs. A process whose start time can't be read is not counted.

The system sounds session never belongs to an app. A session shared by several processes is matched by the process that created it.

Nothing happens when the focused window belongs to the mod's own process or to Explorer, which hosts the desktop, the taskbar and folder windows. Explorer is recognised as the shell's process or any process running the same executable.

## Toggling

All of the app's sessions change together. If any session is unmuted, the press mutes every session; if all are muted, it unmutes them. The mute is the per-session mute the Windows Volume Mixer shows, so Windows keeps it when the app restarts.

## Process and lifetime

The mod is a Windhawk tool mod: it targets `windhawk.exe`, and the standard tool-mod snippet at the end of the file starts a dedicated `windhawk.exe -tool-mod mute-focused-window` process. Nothing is injected into other apps and no functions are hooked.

In that process, one thread owns the hotkey registration, a multithreaded COM apartment and a message loop. Settings changes and unloading reach it as posted thread messages. Audio objects are created per press and released afterwards, never kept in globals.
