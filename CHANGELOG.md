# Changelog

## 2.9.6

### Added
- **Sound Connect conflict guidance** on the status line, with click-to-reconnect
- **Headphone bridge controls** in Customize → Behavior (auto/pin/device pick, Classic/BLE, sync interval, apply & reconnect)
- **Click-to-seek** on volume rails (Line, Studio, Mono)
- **Smarter desktop alerts** using the real device name, plus optional charging and connect/disconnect toasts
- **Custom accent color picker** in Customize → Appearance (Windows color dialog, keeps the preset swatches)

### Improved
- Bridge publishes a short paired-device list for the settings picker (bridge 0.3.35)
- Behavior settings layout and ASCII-safe separators

## 2.9.5

### Added
- **Dark text tone** in Customize → Appearance → Text Tone for light/white wallpapers (ink labels with light control fills)

### Improved
- Faster, smoother widget updates: skin ticks at 200ms, skips redundant `state.ini` parses, and only fully redraws when headphone state or animations actually change
- Settings panel UI refreshes only when options change
- Bridge writes `state.ini` less often when only uptime/latency heartbeats change (bridge 0.3.34)

## 2.9.4

Previous stable release.
