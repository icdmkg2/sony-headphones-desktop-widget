# Sony Headphones Desktop Widget

A transparent Rainmeter widget for controlling compatible Sony headphones directly from the Windows desktop.

The widget is developed and tested around the Sony WH-1000XM5. It can also discover other Sony WH-1000XM, WH-, and WF-series devices, although available controls and compatibility depend on the model.

## Download

The latest stable release is **v2.9.4**.

[Download the Rainmeter installer](https://github.com/icdmkg2/sony-headphones-desktop-widget/releases/latest/download/Sony.Headphones.Desktop.Widget_2.9.4.rmskin) or browse the [release notes](https://github.com/icdmkg2/sony-headphones-desktop-widget/releases/latest).

## Features

- Battery level, charging state, connection status, codec, and firmware details
- Playback controls, track information, and responsive volume adjustment
- Noise cancelling, ambient sound, focus on voice, and ambient-level controls
- Speak-to-Chat, DSEE, wear detection, touch controls, and multipoint toggles
- EQ presets, Clear Bass, five-band EQ, connection priority, and automatic power-off settings
- Compact and expanded layouts with customizable sizing, typography, five live accent colors, and optional rows
- Lock-safe updates: installers carry a bridge payload while the running helper uses a separate runtime copy
- Three transparent designs: Line, Studio, and Mono Signal
- Low-battery desktop alerts, automatic recovery, reconnect controls, and diagnostic information

Unsupported controls are disabled according to the capabilities reported by the connected headphones.

## Requirements

- Windows 10 or later
- Rainmeter 4.5.17 or later
- A paired, compatible Sony Bluetooth headset

## Installation

1. Pair and connect the headphones in Windows Bluetooth settings.
2. Install the `.rmskin` package.
3. Load `SonyXM5\Minimal.ini` in Rainmeter.
4. Right-click the widget and select **Customize widget** to adjust its appearance and behavior.

Sony's private control service accepts only one controller at a time. If the widget cannot connect, close Sony Sound Connect and other Sony headphone-control clients, then use **Reconnect headphones** from the widget menu.

## Configuration

The main bridge settings are stored in:

```text
Skin/SonyXM5/@Resources/Data/settings.ini
```

Leave `DeviceMac=auto` to select a compatible paired headset automatically, or enter a Bluetooth address to pin a specific device.

Classic Bluetooth is the stable default for the WH-1000XM5. The BLE backend is available for testing through `ConnectionMode=ble`.

## Building

Building the native bridge requires Git, CMake 3.31 or later, and Visual Studio 2022 Build Tools with the C++ workload.

```powershell
.\Scripts\Build-Bridge.ps1
```

To create the Rainmeter installer, install `rmskin-builder` 2.0.4 or later and run:

```powershell
pip install rmskin-builder
.\Scripts\Build-Package.ps1
```

The finished `.rmskin` package is written to `dist/`.

## Credits

Headphone communication is powered by [SonyHeadphonesClient](https://github.com/mos9527/SonyHeadphonesClient). Copyright (c) 2026 mos9527, Amr Satrio and other contributors. It is used under the MIT License; the complete notice is included in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) and with the packaged widget.

Manrope is included under the SIL Open Font License.

## Disclaimer

This is an independent project and is not affiliated with or endorsed by Sony. Sony, WH-1000XM, and related product names are trademarks of their respective owner.
