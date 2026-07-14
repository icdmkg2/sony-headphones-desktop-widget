Sony Headphones Desktop Widget
==============================

1. Pair and connect the WH-1000XM5 in Windows Bluetooth settings.
2. Load SonyXM5\Minimal.ini. The local control helper starts automatically.
3. Click MORE to expand the transparent control row; click LESS to collapse it.
4. On the first launch, the welcome guide previews LINE, STUDIO, and MONO
   SIGNAL, saves the selected design, and lets you start compact or expanded.
   Run the guide again from Appearance or Tools whenever you want.
5. Right-click the widget and choose Customize widget to open the live settings
   window. Appearance, Compact, Expanded, Behavior, and Tools are separate
   pages, so layout, visibility, alerts, diagnostics, and placement are easier
   to find. Changes are saved independently and previewed live.
6. Click DEBUG in the settings window for a read-only connected-headphone
   summary, the complete raw bridge state and configuration, and recent log
   lines. REFRESH rereads the files and COPY ALL copies the full report.
7. Use Appearance in settings to switch between LINE, STUDIO, and MONO SIGNAL.
   LINE is text-first, STUDIO uses structured control capsules, and MONO SIGNAL
   is a sharp icon-led layout with thin rails and no permanent button fills.
   Every design keeps the desktop background transparent.
8. In expanded view, click ADVANCED for EQ preset, Clear Bass, five EQ bands,
   multipoint, sound-quality / stable-connection priority, automatic power-off,
   codec / firmware information, and protected two-click headphone power-off.
9. Settings also control smooth motion, low-battery notifications and their
   threshold, and the optional connection-health row. Open DEBUG for connection
   attempts, reconnects, poll errors, and the last disconnect.
   TEST ALERT runs directly from the settings window and shows a small desktop
   toast above the taskbar. Selected designs, enabled controls, active listening
   modes, and a healthy connection use a consistent green status accent.

Compact view shows battery, connection status, listening mode, and playback.
Expanded view adds direct ANC / ambient controls, ambient strength, media,
volume, Speak-to-Chat, DSEE, wear pause, and reconnect.

Volume buttons use relative Sony volume steps, accumulate rapid clicks, and
update the displayed percentage immediately while the headset confirms them.
The percentage also supports mouse-wheel adjustment.

The main skin animates ten times per second while the bridge publishes changes
atomically and keeps a two-second safety heartbeat. Playback, listening mode,
ambient strength, volume, and
quick toggles render immediately on click while confirmation is in flight.
Volume fills ease to their new value, expand / collapse height is smoothed, and
the connection dot pulses only during commands, recovery, or errors.

The connection-health row reports measured transport, codec, command-queue
latency, uptime, connect time, attempts, successful reconnects, and poll errors.
It does not invent a Bluetooth signal-strength value the Sony control protocol
does not provide.

Low-battery alerts trigger once when the configured threshold is crossed and
re-arm after charging or recovering above the threshold. The alert uses its own
non-focus-stealing desktop toast instead of a legacy tray balloon, with a basic
popup fallback. The battery indicator also changes colour while power is low.

The widget has no visible background. Drag any empty space to move it.
After dragging, its full interactive bounds are clamped to the current monitor's
Windows work area, so the compact or expanded widget cannot cover the taskbar.

Manrope is bundled with the skin, so its typography does not depend on which
fonts are installed in Windows.

The bridge uses the Classic Sony control service by default and stays running
independently when the Rainmeter skin reloads. It does not automatically jump
to BLE. A previous install's automatic setting is treated as classic.

Automatic full-state refreshes are disabled by default because an unnecessary
idle sync can destabilize the private Sony control service. Live events and
control changes still update normally, and Refresh headphone state remains in
the right-click menu for an explicit resync.

Short-lived polling errors are held briefly instead of immediately dropping the
control link. Persistent failures enter a silent 30-second recovery window,
reuse the last known headphone address, and keep the last valid battery and
control state visible while reconnecting. The packaged Lua reader runs in
Unicode mode so track and artist metadata is passed to Rainmeter as UTF-8
instead of mojibake.

The private Sony control service accepts one controller at a time. Close
SonyHeadphonesClient and Sony Sound Connect on nearby devices if another
controller has claimed it, then choose Reconnect from the expanded view or
the skin's right-click menu.

To pin a specific paired headset, edit @Resources\Data\settings.ini and set
DeviceMac to its Bluetooth address.

This independent project is not affiliated with or endorsed by Sony.
The control bridge uses the MIT-licensed mos9527/SonyHeadphonesClient library.
Copyright (c) 2026 mos9527, Amr Satrio and other contributors.
The complete license is included at Licenses\SonyHeadphonesClient-LICENSE.txt.
