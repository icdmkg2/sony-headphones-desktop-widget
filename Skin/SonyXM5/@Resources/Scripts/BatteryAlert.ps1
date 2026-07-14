param(
    [ValidateRange(0, 100)]
    [int]$Battery = 20,

    [switch]$Test
)

$ErrorActionPreference = 'Stop'
$title = if ($Test) { 'Battery alert test' } else { 'WH-1000XM5 battery low' }
$message = if ($Test) {
    "Notifications are working. The current alert threshold is $Battery%."
} else {
    "Headphone battery is at $Battery%. Connect a charger soon."
}

try {
    Add-Type -AssemblyName PresentationFramework
    Add-Type -AssemblyName PresentationCore
    Add-Type -AssemblyName WindowsBase

    $window = New-Object System.Windows.Window
    $window.Width = 380
    $window.Height = 96
    $window.WindowStyle = [System.Windows.WindowStyle]::None
    $window.ResizeMode = [System.Windows.ResizeMode]::NoResize
    $window.AllowsTransparency = $true
    $window.Background = [System.Windows.Media.Brushes]::Transparent
    $window.ShowInTaskbar = $false
    $window.ShowActivated = $false
    $window.Focusable = $false
    $window.Topmost = $true
    $window.Opacity = 0

    $workArea = [System.Windows.SystemParameters]::WorkArea
    $window.Left = $workArea.Right - $window.Width - 24
    $window.Top = $workArea.Bottom - $window.Height - 24

    $panel = New-Object System.Windows.Controls.Border
    $panel.CornerRadius = [System.Windows.CornerRadius]::new(14)
    $panel.Background = [System.Windows.Media.SolidColorBrush]::new([System.Windows.Media.Color]::FromArgb(248, 24, 24, 27))
    $panel.BorderBrush = [System.Windows.Media.SolidColorBrush]::new([System.Windows.Media.Color]::FromArgb(255, 72, 72, 78))
    $panel.BorderThickness = [System.Windows.Thickness]::new(1)

    $shadow = New-Object System.Windows.Media.Effects.DropShadowEffect
    $shadow.BlurRadius = 22
    $shadow.ShadowDepth = 5
    $shadow.Opacity = 0.4
    $shadow.Color = [System.Windows.Media.Colors]::Black
    $panel.Effect = $shadow

    $grid = New-Object System.Windows.Controls.Grid
    $accent = New-Object System.Windows.Controls.Border
    $accent.Width = 4
    $accent.HorizontalAlignment = [System.Windows.HorizontalAlignment]::Left
    $accent.CornerRadius = [System.Windows.CornerRadius]::new(14, 0, 0, 14)
    $accent.Background = [System.Windows.Media.SolidColorBrush]::new([System.Windows.Media.Color]::FromRgb(255, 115, 115))
    $null = $grid.Children.Add($accent)

    $content = New-Object System.Windows.Controls.StackPanel
    $content.Margin = [System.Windows.Thickness]::new(24, 14, 18, 12)

    $heading = New-Object System.Windows.Controls.TextBlock
    $heading.Text = $title
    $heading.FontFamily = [System.Windows.Media.FontFamily]::new('Segoe UI Variable Text')
    $heading.FontSize = 14
    $heading.FontWeight = [System.Windows.FontWeights]::SemiBold
    $heading.Foreground = [System.Windows.Media.SolidColorBrush]::new([System.Windows.Media.Color]::FromRgb(245, 245, 245))
    $null = $content.Children.Add($heading)

    $body = New-Object System.Windows.Controls.TextBlock
    $body.Text = $message
    $body.Margin = [System.Windows.Thickness]::new(0, 7, 0, 0)
    $body.FontFamily = [System.Windows.Media.FontFamily]::new('Segoe UI Variable Text')
    $body.FontSize = 11
    $body.TextWrapping = [System.Windows.TextWrapping]::Wrap
    $body.Foreground = [System.Windows.Media.SolidColorBrush]::new([System.Windows.Media.Color]::FromRgb(175, 175, 182))
    $null = $content.Children.Add($body)
    $null = $grid.Children.Add($content)
    $panel.Child = $grid
    $window.Content = $panel

    $fadeIn = New-Object System.Windows.Media.Animation.DoubleAnimation
    $fadeIn.From = 0
    $fadeIn.To = 1
    $fadeIn.Duration = [System.Windows.Duration]::new([TimeSpan]::FromMilliseconds(180))
    $window.BeginAnimation([System.Windows.Window]::OpacityProperty, $fadeIn)

    $timer = New-Object System.Windows.Threading.DispatcherTimer
    $timer.Interval = [TimeSpan]::FromSeconds(5)
    $timer.Add_Tick({
        $timer.Stop()
        $fadeOut = New-Object System.Windows.Media.Animation.DoubleAnimation
        $fadeOut.From = 1
        $fadeOut.To = 0
        $fadeOut.Duration = [System.Windows.Duration]::new([TimeSpan]::FromMilliseconds(240))
        $fadeOut.Add_Completed({ $window.Close() })
        $window.BeginAnimation([System.Windows.Window]::OpacityProperty, $fadeOut)
    })
    $timer.Start()
    $null = $window.ShowDialog()
}
catch {
    try {
        $shell = New-Object -ComObject WScript.Shell
        $null = $shell.Popup($message, 6, $title, 48)
    }
    catch {
        exit 0
    }
}
