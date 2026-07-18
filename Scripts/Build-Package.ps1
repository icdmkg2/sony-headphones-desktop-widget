[CmdletBinding()]
param(
    [string]$Version = '2.9.6'
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$StageRoot = Join-Path $RepoRoot '.build\package'
$StageSkin = Join-Path $StageRoot 'Skins\SonyXM5'
$DistRoot = Join-Path $RepoRoot 'dist'
$Bridge = Join-Path $RepoRoot 'Skin\SonyXM5\@Resources\Bridge\SonyXM5Bridge-0.3.35.bin'
$BundledBuilder = Join-Path $RepoRoot '.build\rmskin-tools\bin\rmskin-builder.exe'

if (-not (Test-Path $Bridge)) {
    throw 'SonyXM5Bridge-0.3.35.bin is missing. Run Scripts\Build-Bridge.ps1 first.'
}

$Builder = Get-Command 'rmskin-builder' -ErrorAction SilentlyContinue
if ($Builder) {
    $BuilderPath = $Builder.Source
} elseif (Test-Path $BundledBuilder) {
    $BuilderPath = $BundledBuilder
    $env:PYTHONPATH = (Join-Path $RepoRoot '.build\rmskin-tools')
} else {
    throw 'rmskin-builder 2.0.4+ is required. Install it with: pip install rmskin-builder'
}

if (Test-Path $StageRoot) {
    $ResolvedRepo = [IO.Path]::GetFullPath($RepoRoot).TrimEnd('\')
    $ResolvedStage = [IO.Path]::GetFullPath($StageRoot).TrimEnd('\')
    if (-not $ResolvedStage.StartsWith($ResolvedRepo + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to replace a staging directory outside the repository: $ResolvedStage"
    }
    Remove-Item -Recurse -Force -LiteralPath $StageRoot
}

New-Item -ItemType Directory -Force -Path $StageSkin, $DistRoot | Out-Null
Copy-Item -Recurse -Force -Path (Join-Path $RepoRoot 'Skin\SonyXM5\*') -Destination $StageSkin
Copy-Item -Force -LiteralPath (Join-Path $RepoRoot 'Packaging\RMSKIN.ini') -Destination (Join-Path $StageRoot 'RMSKIN.ini')
# state.ini is live bridge output. Never package it over an existing bridge;
# on a fresh install the bridge creates it immediately after launch.
Remove-Item -Force -ErrorAction SilentlyContinue -LiteralPath (Join-Path $StageSkin '@Resources\Data\state.ini')
Get-ChildItem (Join-Path $StageSkin '@Resources\Bridge') -File -Filter 'SonyXM5Bridge*' -ErrorAction SilentlyContinue |
    Where-Object Name -ne 'SonyXM5Bridge-0.3.35.bin' |
    Remove-Item -Force
Remove-Item -Recurse -Force -ErrorAction SilentlyContinue -LiteralPath (Join-Path $StageSkin '@Resources\Data\Runtime')

Get-ChildItem (Join-Path $StageSkin '@Resources\Scripts') -Filter '*.lua' | ForEach-Object {
    $LuaText = [IO.File]::ReadAllText($_.FullName, (New-Object Text.UTF8Encoding($false)))
    [IO.File]::WriteAllText($_.FullName, $LuaText, (New-Object Text.UnicodeEncoding($false, $true)))
}

Remove-Item -Force -ErrorAction SilentlyContinue -LiteralPath (Join-Path $StageSkin '@Resources\Data\bridge.log')
Remove-Item -Force -ErrorAction SilentlyContinue -LiteralPath (Join-Path $StageSkin '@Resources\Data\state.ini.tmp')
Get-ChildItem (Join-Path $StageSkin '@Resources\Data\Queue') -Filter '*.cmd' -ErrorAction SilentlyContinue |
    Remove-Item -Force
Get-ChildItem $StageSkin -Recurse -Filter '.gitkeep' -ErrorAction SilentlyContinue |
    Remove-Item -Force

& $BuilderPath --path $StageRoot --dir-out $DistRoot --version $Version --title 'Sony-Headphones-Desktop-Widget' --author 'icdmkg'
if ($LASTEXITCODE -ne 0) { throw 'Rainmeter package creation failed.' }

$Package = Get-ChildItem $DistRoot -Filter '*.rmskin' |
    Sort-Object LastWriteTime -Descending |
    Select-Object -First 1
if (-not $Package) { throw 'Package builder completed but no .rmskin file was created.' }

Write-Host "Installer ready: $($Package.FullName)" -ForegroundColor Green
