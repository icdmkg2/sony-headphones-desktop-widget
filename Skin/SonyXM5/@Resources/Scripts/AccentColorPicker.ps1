param(
    [string]$Current = '91,225,145,255'
)

$ErrorActionPreference = 'Stop'

try {
    Add-Type -AssemblyName System.Windows.Forms
    Add-Type -AssemblyName System.Drawing

    $parts = @($Current -split '\s*,\s*')
    $red = 91
    $green = 225
    $blue = 145
    if ($parts.Count -ge 3) {
        $red = [Math]::Max(0, [Math]::Min(255, [int]$parts[0]))
        $green = [Math]::Max(0, [Math]::Min(255, [int]$parts[1]))
        $blue = [Math]::Max(0, [Math]::Min(255, [int]$parts[2]))
    }

    $dialog = New-Object System.Windows.Forms.ColorDialog
    $dialog.AllowFullOpen = $true
    $dialog.FullOpen = $true
    $dialog.AnyColor = $true
    $dialog.SolidColorOnly = $true
    $dialog.Color = [System.Drawing.Color]::FromArgb(255, $red, $green, $blue)
    $dialog.CustomColors = @(
        [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::FromArgb(91, 225, 145)),
        [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::FromArgb(84, 200, 255)),
        [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::FromArgb(181, 143, 255)),
        [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::FromArgb(255, 190, 92)),
        [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::FromArgb(255, 117, 154))
    )

    $result = $dialog.ShowDialog()
    if ($result -eq [System.Windows.Forms.DialogResult]::OK) {
        Write-Output ('{0},{1},{2},255' -f $dialog.Color.R, $dialog.Color.G, $dialog.Color.B)
    }
}
catch {
    exit 0
}
