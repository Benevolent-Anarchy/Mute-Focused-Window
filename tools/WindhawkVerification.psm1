#requires -Version 7.2
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
# Project checks for the Windhawk mods under mods/. The build stage mirrors the
# installed Windhawk editor's own compile command (extensions/windhawk/dist/extension.js,
# compileModInternal) so a pass here means the editor accepts the same source. Warnings
# follow the editor's clangd flags and are made fatal.

$script:EditorDefines=@('-DUNICODE','-D_UNICODE','-DWINVER=0x0A00','-D_WIN32_WINNT=0x0A00','-D_WIN32_IE=0x0A00','-DNTDDI_VERSION=0x0A000008','-D__USE_MINGW_ANSI_STDIO=0','-DWH_MOD')
$script:EditorWarnings=@('-Wall','-Wextra','-Wno-unused-parameter','-Wno-missing-field-initializers','-Wno-cast-function-type-mismatch','-Werror')
# @architecture value -> clang target and the engine's library subfolder. An empty list means
# x86 and x86-64, as in the editor.
$script:Targets=@{'x86'=@('i686-w64-mingw32','32');'x86-64'=@('x86_64-w64-mingw32','64');'amd64'=@('x86_64-w64-mingw32','64');'arm64'=@('aarch64-w64-mingw32','arm64')}

function Resolve-WindhawkToolchain {
    # WINDHAWK_ROOT overrides the default installation folder, e.g. for a portable copy.
    $root=if ($env:WINDHAWK_ROOT) {$env:WINDHAWK_ROOT} else {Join-Path $env:ProgramFiles 'Windhawk'}
    $ini=Join-Path $root 'windhawk.ini'
    if (-not (Test-Path -LiteralPath $ini -PathType Leaf)) {throw "Windhawk is not installed at '$root'. Install Windhawk or set WINDHAWK_ROOT to its folder."}
    $values=@{}
    # ReadAllLines honours the UTF-16 byte-order mark Windhawk writes.
    foreach ($line in [IO.File]::ReadAllLines($ini)) {if ($line -match '^\s*([A-Za-z]+)\s*=\s*(.*?)\s*$') {$values[$Matches[1]]=$Matches[2]}}
    foreach ($key in 'CompilerPath','EnginePath') {if (-not $values.ContainsKey($key)) {throw "windhawk.ini has no $key."}}
    $compiler=[IO.Path]::Combine($root,$values.CompilerPath);$engine=[IO.Path]::Combine($root,$values.EnginePath)
    $clang=Join-Path $compiler 'bin/clang++.exe'
    if (-not (Test-Path -LiteralPath $clang -PathType Leaf)) {throw "Windhawk compiler not found: $clang"}
    @{Root=$root;Compiler=$compiler;Engine=$engine;Clang=$clang}
}

function Get-WindhawkBlock([string[]]$Lines,[string]$Name) {
    $start=@(0..($Lines.Count-1) | Where-Object {$Lines[$_].Trim() -ceq "// ==$Name=="})
    $end=@(0..($Lines.Count-1) | Where-Object {$Lines[$_].Trim() -ceq "// ==/$Name=="})
    if ($start.Count -ne 1 -or $end.Count -ne 1 -or $end[0] -le $start[0]) {return $null}
    ,@($Lines[($start[0]+1)..($end[0]-1)])
}

function Read-WindhawkMod([string]$Path) {
    $text=[IO.File]::ReadAllText($Path);$lines=$text -split '\r?\n'
    $metadata=[ordered]@{}
    $block=Get-WindhawkBlock $lines 'WindhawkMod'
    if ($null -ne $block) {
        foreach ($line in $block) {
            if ($line -match '^//\s*@(\S+)\s+(.*?)\s*$') {
                if (-not $metadata.Contains($Matches[1])) {$metadata[$Matches[1]]=[Collections.Generic.List[string]]::new()}
                $metadata[$Matches[1]].Add($Matches[2])
            }
        }
    }
    @{Path=$Path;Text=$text;HasMetadata=($null -ne $block);Metadata=$metadata;Readme=(Get-WindhawkBlock $lines 'WindhawkModReadme');Settings=(Get-WindhawkBlock $lines 'WindhawkModSettings')}
}

function Get-WindhawkValue($Mod,[string]$Key) {if ($Mod.Metadata.Contains($Key)) {$Mod.Metadata[$Key][0]} else {$null}}

function Split-CompilerOptions([string]$Options) {
    # Whitespace-separated, with single or double quotes grouping, as the editor splits them.
    if (-not $Options) {return @()}
    @([regex]::Matches($Options,'(?:"[^"]*"|''[^'']*''|[^\s"'']+)+') | ForEach-Object {$_.Value -replace '["'']',''})
}

function Get-WindhawkTargets($Mod) {
    $architectures=if ($Mod.Metadata.Contains('architecture')) {@($Mod.Metadata['architecture'])} else {@('x86','x86-64')}
    $targets=[ordered]@{}
    foreach ($architecture in $architectures) {if ($script:Targets.ContainsKey($architecture)) {$targets[$script:Targets[$architecture][0]]=$script:Targets[$architecture]}}
    ,@($targets.Values)
}

function Test-WindhawkModMetadata($Mod) {
    $problems=[Collections.Generic.List[string]]::new();$file=Split-Path $Mod.Path -Leaf
    if (-not $Mod.HasMetadata) {return @("${file}: missing a single // ==WindhawkMod== block.")}
    foreach ($key in 'id','name','description','version','author','include') {if (-not (Get-WindhawkValue $Mod $key)) {$problems.Add("${file}: missing @$key.")}}
    $id=Get-WindhawkValue $Mod 'id'
    if ($id -and $id -cnotmatch '^[a-z0-9]+(-[a-z0-9]+)*$') {$problems.Add("${file}: @id '$id' must be lowercase words separated by hyphens.")}
    if ($id -and $file -cne "$id.wh.cpp") {$problems.Add("${file}: file name must be '$id.wh.cpp'.")}
    $version=Get-WindhawkValue $Mod 'version'
    if ($version -and $version -notmatch '^\d+(\.\d+){0,3}$') {$problems.Add("${file}: @version '$version' must be numeric, such as 1.0 or 1.2.3.")}
    if ($Mod.Metadata.Contains('architecture')) {foreach ($architecture in $Mod.Metadata['architecture']) {if (-not $script:Targets.ContainsKey($architecture)) {$problems.Add("${file}: unsupported @architecture '$architecture'.")}}}
    if ($null -eq $Mod.Readme) {$problems.Add("${file}: missing a single // ==WindhawkModReadme== block.")}
    # Every setting the code reads must be declared, so the Windhawk UI can show and store it.
    $used=@([regex]::Matches($Mod.Text,'Wh_Get(?:Int|String)Setting\s*\(\s*L"([A-Za-z0-9_]+)') | ForEach-Object {$_.Groups[1].Value} | Sort-Object -Unique)
    if ($used.Count) {
        if ($null -eq $Mod.Settings) {$problems.Add("${file}: reads settings but has no // ==WindhawkModSettings== block.")}
        else {
            $declared=@($Mod.Settings | Where-Object {$_ -match '^-\s*([A-Za-z0-9_]+)\s*:'} | ForEach-Object {$Matches[1]})
            foreach ($name in $used) {if ($name -cnotin $declared) {$problems.Add("${file}: setting '$name' is read but not declared.")}}
        }
    }
    $problems.ToArray()
}

function Invoke-WindhawkVerification([string]$Repository,[hashtable]$Config,[scriptblock]$Process) {
    $stages=[Collections.Generic.List[object]]::new()
    $mods=@(Get-ChildItem -LiteralPath (Join-Path $Repository 'mods') -Filter '*.wh.cpp' -File -ErrorAction SilentlyContinue | Sort-Object Name | ForEach-Object {Read-WindhawkMod $_.FullName})
    $output=Join-Path $Repository '.test-results/windhawk'

    $metadataProblems=@(if (-not $mods.Count) {'mods/: no *.wh.cpp mod found.'} else {foreach ($mod in $mods) {Test-WindhawkModMetadata $mod}})
    $stages.Add(@{Name='windhawk-metadata';Passed=($metadataProblems.Count -eq 0);ExitCode=$(if($metadataProblems.Count){1}else{0});Diagnostics=$metadataProblems})

    try {$toolchain=Resolve-WindhawkToolchain}
    catch {
        foreach ($name in 'windhawk-build','windhawk-tests') {$stages.Add(@{Name=$name;Passed=$false;ExitCode=1;Diagnostics=@($_.Exception.Message)})}
        return @{Stages=$stages.ToArray();Diagnostics=@()}
    }

    $buildProblems=[Collections.Generic.List[string]]::new()
    if (-not $mods.Count) {$buildProblems.Add('mods/: nothing to build.')}
    foreach ($mod in $mods) {
        $id=Get-WindhawkValue $mod 'id';$version=Get-WindhawkValue $mod 'version'
        if (-not $id -or -not $version) {$buildProblems.Add("$(Split-Path $mod.Path -Leaf): build needs @id and @version.");continue}
        foreach ($target in (Get-WindhawkTargets $mod)) {
            $dll=Join-Path $output "$($target[1])/$id.dll"
            $null=New-Item -ItemType Directory -Force -Path (Split-Path $dll)
            $arguments=@('-std=c++23','-O2','-shared')+$script:EditorDefines+@("-DWH_MOD_ID=L`"$id`"","-DWH_MOD_VERSION=L`"$version`"",(Join-Path $toolchain.Engine "$($target[1])/windhawk.lib"),'-x','c++',$mod.Path,'-include','windhawk_api.h','-target',$target[0],'-Wl,--export-all-symbols','-o',$dll)+$script:EditorWarnings+(Split-CompilerOptions (Get-WindhawkValue $mod 'compilerOptions'))
            $result=& $Process $toolchain.Clang $arguments $toolchain.Compiler @{}
            if ($result.ExitCode -ne 0) {$buildProblems.Add("$id ($($target[0])) failed to compile:`n$($result.Output.Trim())")}
        }
    }
    $stages.Add(@{Name='windhawk-build';Passed=($buildProblems.Count -eq 0);ExitCode=$(if($buildProblems.Count){1}else{0});Diagnostics=$buildProblems.ToArray()})

    # tests/<mod-id>.test.cpp includes that mod's source with the editor's no-op API stubs
    # (WH_EDITING) and exits nonzero on failure. It links the mod's own @compilerOptions.
    $testProblems=[Collections.Generic.List[string]]::new()
    $tests=@(Get-ChildItem -LiteralPath (Join-Path $Repository 'tests') -Filter '*.test.cpp' -File -ErrorAction SilentlyContinue | Sort-Object Name)
    if (-not $tests.Count) {$testProblems.Add('tests/: no *.test.cpp found.')}
    foreach ($test in $tests) {
        $id=$test.Name -replace '\.test\.cpp$',''
        $mod=@($mods | Where-Object {(Get-WindhawkValue $_ 'id') -ceq $id})
        if ($mod.Count -ne 1) {$testProblems.Add("$($test.Name): no mod with @id '$id'.");continue}
        $exe=Join-Path $output "tests/$id.test.exe"
        $null=New-Item -ItemType Directory -Force -Path (Split-Path $exe)
        $arguments=@('-std=c++23','-O1','-static')+$script:EditorDefines+@('-DWH_EDITING','-include','windhawk_api.h','-target','x86_64-w64-mingw32','-x','c++',$test.FullName,'-o',$exe)+$script:EditorWarnings+(Split-CompilerOptions (Get-WindhawkValue $mod[0] 'compilerOptions'))
        $build=& $Process $toolchain.Clang $arguments $toolchain.Compiler @{}
        if ($build.ExitCode -ne 0) {$testProblems.Add("$($test.Name) failed to compile:`n$($build.Output.Trim())");continue}
        $run=& $Process $exe @() $Repository @{}
        if ($run.ExitCode -ne 0) {$testProblems.Add("$($test.Name) failed (exit $($run.ExitCode)):`n$($run.Output.Trim())")}
    }
    $stages.Add(@{Name='windhawk-tests';Passed=($testProblems.Count -eq 0);ExitCode=$(if($testProblems.Count){1}else{0});Diagnostics=$testProblems.ToArray()})
    @{Stages=$stages.ToArray();Diagnostics=@()}
}
Export-ModuleMember -Function Invoke-WindhawkVerification,Read-WindhawkMod,Test-WindhawkModMetadata,Resolve-WindhawkToolchain
