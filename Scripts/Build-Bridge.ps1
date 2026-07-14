[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$CMakeRepoRoot = $RepoRoot.Replace('\', '/')
$BuildRoot = Join-Path $RepoRoot '.build'
$UpstreamRoot = Join-Path $BuildRoot 'SonyHeadphonesClient'
$UpstreamCommit = 'eff6a9101193f41a33c38f4aee6037fd698b80c3'
$UpstreamUrl = 'https://github.com/mos9527/SonyHeadphonesClient.git'
$BuildDirectory = Join-Path $BuildRoot "bridge-msvc-$($Configuration.ToLowerInvariant())"
$BridgeDestination = Join-Path $RepoRoot 'Skin\SonyXM5\@Resources\Bridge\SonyXM5Bridge-0.3.33.bin'

function Assert-Command([string]$Name, [string]$Help) {
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        throw "$Name was not found. $Help"
    }
}

$GitCommand = Get-Command 'git' -ErrorAction SilentlyContinue
if (-not $GitCommand) {
    throw 'Git was not found. Install Git for Windows and reopen PowerShell.'
}

$CMakeCommand = Get-Command 'cmake' -ErrorAction SilentlyContinue
$BundledCMake = Join-Path $BuildRoot 'portable-tools\cmake\data\bin\cmake.exe'
if ($CMakeCommand) {
    $CMakeExe = $CMakeCommand.Source
} elseif (Test-Path $BundledCMake) {
    $CMakeExe = $BundledCMake
} else {
    throw 'CMake 3.31 or newer was not found.'
}

$VsWhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $VsWhere)) {
    throw 'Visual Studio 2022 Build Tools with the C++ workload is required.'
}
$VisualStudio = & $VsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $VisualStudio) {
    throw 'Visual Studio 2022 Build Tools with the C++ workload is required.'
}

$CMakeVersion = (& $CMakeExe --version | Select-Object -First 1) -replace '^cmake version\s+', ''
if ([version]$CMakeVersion -lt [version]'3.31.0') {
    throw "CMake 3.31 or newer is required by SonyHeadphonesClient. Found $CMakeVersion."
}

if ($Clean -and (Test-Path $BuildRoot)) {
    $ResolvedRepo = [IO.Path]::GetFullPath($RepoRoot).TrimEnd('\')
    $ResolvedBuild = [IO.Path]::GetFullPath($BuildRoot).TrimEnd('\')
    if (-not $ResolvedBuild.StartsWith($ResolvedRepo + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to clean a directory outside the repository: $ResolvedBuild"
    }
    Remove-Item -Recurse -Force -ErrorAction SilentlyContinue -LiteralPath $UpstreamRoot, $BuildDirectory
}

New-Item -ItemType Directory -Force -Path $BuildRoot | Out-Null
if (-not (Test-Path (Join-Path $UpstreamRoot '.git'))) {
    & $GitCommand.Source clone --filter=blob:none --no-checkout $UpstreamUrl $UpstreamRoot
}

& $GitCommand.Source -C $UpstreamRoot fetch --depth 1 origin $UpstreamCommit
& $GitCommand.Source -C $UpstreamRoot reset --hard $UpstreamCommit
& $GitCommand.Source -C $UpstreamRoot clean -fdx

$InjectedBridge = Join-Path $UpstreamRoot 'widget-bridge'
Copy-Item -Recurse -Force -Path (Join-Path $RepoRoot 'Bridge') -Destination $InjectedBridge

$RootCMake = Join-Path $UpstreamRoot 'CMakeLists.txt'
$CMakeText = [IO.File]::ReadAllText($RootCMake)
$CMakeText = $CMakeText.Replace(
    'set(CMAKE_CXX_STANDARD 20)',
    "set(CMAKE_CXX_STANDARD 20)`r`n`r`nif (CMAKE_CXX_COMPILER_ID STREQUAL `"MSVC`")`r`n    add_compile_options(`"/experimental:deterministic`")`r`n    add_compile_options(`"/pathmap:$CMakeRepoRoot=.`")`r`nendif()"
)
$CMakeText = $CMakeText.Replace(
    'add_subdirectory(client)',
    "option(MDR_BUILD_CLIENT `"Build the reference GUI client`" OFF)`r`nif(MDR_BUILD_CLIENT)`r`n    add_subdirectory(client)`r`nendif()"
)
if ($CMakeText -notmatch 'add_subdirectory\(widget-bridge\)') {
    $CMakeText = $CMakeText.TrimEnd() + "`r`nadd_subdirectory(widget-bridge)`r`n"
    [IO.File]::WriteAllText($RootCMake, $CMakeText, (New-Object Text.UTF8Encoding($false)))
}

New-Item -ItemType Directory -Force -Path $BuildDirectory | Out-Null
& $CMakeExe -S $UpstreamRoot -B $BuildDirectory -G 'Visual Studio 17 2022' -A x64 '-DMDR_ENABLE_CODEGEN=OFF' '-DMDR_BUILD_CLIENT=OFF' '-DCMAKE_CXX_SCAN_FOR_MODULES=OFF' '-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>'
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }

& $CMakeExe --build $BuildDirectory --target SonyXM5Bridge --config $Configuration --parallel
if ($LASTEXITCODE -ne 0) { throw 'Bridge compilation failed.' }

$Executable = Get-ChildItem -Path $BuildDirectory -Recurse -Filter 'SonyXM5Bridge.exe' |
    Sort-Object LastWriteTime -Descending |
    Select-Object -First 1
if (-not $Executable) { throw 'The bridge built successfully, but SonyXM5Bridge.exe was not found.' }

New-Item -ItemType Directory -Force -Path (Split-Path -Parent $BridgeDestination) | Out-Null
Copy-Item -Force -LiteralPath $Executable.FullName -Destination $BridgeDestination
Write-Host "Bridge ready: $BridgeDestination" -ForegroundColor Green
