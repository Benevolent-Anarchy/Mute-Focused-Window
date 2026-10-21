# Verification decisions

The gate is [tools/verify.ps1](../../tools/verify.ps1). The mod itself is described in [[design/mod|Mod behaviour]].

## 2026-10-21

### Build mods with the installed Windhawk compiler

The `windhawk-build` stage compiles each mod with the Windhawk installation's own clang. It uses the arguments the Windhawk 1.7.3 editor passes: `compileModInternal` in the installed `extensions/windhawk/dist/extension.js`. It builds every target the mod's `@architecture` selects; with none, that is x86 and x86-64, as in the editor. It adds the editor's clangd warning set with `-Werror`.

A plain syntax check was rejected because only the full build catches missing link libraries and target-specific errors. The x86 build has already caught a lambda passed as a `CALLBACK` function, which compiles for x86-64 only. A separate compiler such as MSVC was rejected: Windhawk builds mods with llvm-mingw, and differences in headers and libraries would let failures through.

The gate therefore requires Windhawk on the machine. It reads `windhawk.ini` in `%ProgramFiles%\Windhawk`, or in the folder named by the `WINDHAWK_ROOT` environment variable for a portable install.

### Unit test pure logic by including the mod

`tests/<mod-id>.test.cpp` includes the mod's source and is built with the editor's no-op API stubs (`WH_EDITING`), then run. The parsing, process-family and toggle rules are written as pure functions so this covers them without Windhawk, audio devices or other processes.

Splitting the logic into a separate header was rejected because Windhawk mods are a single file. Testing the whole mod automatically was rejected: running it needs the Windhawk app, and checking its effect needs real windows, processes and audio. That behaviour needs a human check.

### Check mod metadata in the gate

The `windhawk-metadata` stage checks the metadata block's required fields and that `@id` matches the file name. It checks the version format and `@architecture` values, that the readme block exists, and that every setting the code reads is declared.

These mistakes are otherwise found only when Windhawk loads the mod or when the windhawk-mods review rejects it. The full windhawk-mods submission validator was not copied: its rules change upstream, and some depend on the submitting GitHub account.
