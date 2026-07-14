[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$PayloadPath,
    [Parameter(Mandatory = $true)]
    [string]$RuntimeDirectory,
    [Parameter(Mandatory = $true)]
    [string]$DataPath
)

$ErrorActionPreference = 'Stop'
$PayloadPath = [IO.Path]::GetFullPath($PayloadPath)
$RuntimeDirectory = [IO.Path]::GetFullPath($RuntimeDirectory)
$DataPath = [IO.Path]::GetFullPath($DataPath)

if (-not (Test-Path -LiteralPath $PayloadPath -PathType Leaf)) {
    throw "Bridge payload is missing: $PayloadPath"
}

New-Item -ItemType Directory -Path $RuntimeDirectory -Force | Out-Null
$PayloadHash = (Get-FileHash -LiteralPath $PayloadPath -Algorithm SHA256).Hash
$RuntimePath = Join-Path $RuntimeDirectory "SonyXM5Bridge-$($PayloadHash.Substring(0, 16)).exe"

if (-not (Test-Path -LiteralPath $RuntimePath -PathType Leaf)) {
    $TemporaryPath = "$RuntimePath.tmp-$PID"
    try {
        Copy-Item -LiteralPath $PayloadPath -Destination $TemporaryPath -Force
        try {
            [IO.File]::Move($TemporaryPath, $RuntimePath)
        }
        catch {
            if (-not (Test-Path -LiteralPath $RuntimePath -PathType Leaf)) { throw }
        }
    }
    finally {
        Remove-Item -LiteralPath $TemporaryPath -Force -ErrorAction SilentlyContinue
    }
}

Unblock-File -LiteralPath $RuntimePath -ErrorAction SilentlyContinue
& $RuntimePath '--data-dir' $DataPath

# Old packaged executables and payloads are no longer installation targets.
# Cleanup is best-effort because a previous bridge can remain locked briefly
# while the widget's existing version-upgrade handshake shuts it down.
$BridgeDirectory = Split-Path -Parent $PayloadPath
Get-ChildItem -LiteralPath $BridgeDirectory -File -Filter 'SonyXM5Bridge*' -ErrorAction SilentlyContinue |
    Where-Object { $_.FullName -ne $PayloadPath -and $_.Extension -in @('.exe', '.bin') } |
    Remove-Item -Force -ErrorAction SilentlyContinue
Get-ChildItem -LiteralPath $RuntimeDirectory -File -Filter 'SonyXM5Bridge-*.exe' -ErrorAction SilentlyContinue |
    Where-Object FullName -ne $RuntimePath |
    Remove-Item -Force -ErrorAction SilentlyContinue
