local dataPath
local settingsPath
local logPath
local lastReport = ''
local copyTicks = 0

local CRLF = '\r\n'

local stateOrder = {
    'bridge_version', 'status', 'status_text', 'device_name', 'device_mac',
    'transport', 'error', 'last_command', 'connection_uptime_seconds',
    'connect_latency_ms', 'command_latency_ms', 'connection_attempts',
    'reconnect_count', 'poll_error_count', 'last_disconnect',
    'battery', 'charging', 'volume',
    'playback', 'track_title', 'track_artist', 'codec', 'anc_mode',
    'ambient_level', 'focus_voice', 'speak_to_chat', 'dsee', 'auto_pause',
    'touch_panel', 'multipoint', 'eq_preset', 'eq_bass', 'eq_band_1',
    'eq_band_2', 'eq_band_3', 'eq_band_4', 'eq_band_5', 'priority',
    'auto_off', 'button_function', 'touch_left', 'touch_right', 'firmware',
    'supported_anc', 'supported_ambient', 'supported_eq',
    'supported_speak_to_chat', 'supported_auto_pause',
    'supported_touch_panel', 'supported_multipoint', 'supported_assignable',
    'supported_power_off'
}

local settingsOrder = {'devicemac', 'connectionmode', 'refreshseconds'}

local function readSection(path, wantedSection)
    local values = {}
    local file = io.open(path, 'r')
    if not file then return values end
    local active = false
    for line in file:lines() do
        line = line:gsub('\r$', '')
        local section = line:match('^%s*%[([^]]+)%]')
        if section then
            active = section:lower() == wantedSection:lower()
        elseif active and not line:match('^%s*[;#]') then
            local key, value = line:match('^%s*([^=]+)%s*=(.*)$')
            if key then
                key = key:lower():gsub('%s+$', '')
                values[key] = value:gsub('^%s+', ''):gsub('%s+$', '')
            end
        end
    end
    file:close()
    return values
end

local function readLogTail(path, maximum)
    local file = io.open(path, 'r')
    if not file then return {'No bridge log has been created yet.'} end
    local lines = {}
    for line in file:lines() do
        line = line:gsub('\r$', '')
        if line ~= '' then
            table.insert(lines, line)
            if #lines > maximum then table.remove(lines, 1) end
        end
    end
    file:close()
    if #lines == 0 then return {'Bridge log is empty.'} end
    return lines
end

local function value(values, key, fallback)
    local result = values[key]
    if result == nil or result == '' then return fallback or '--' end
    return result
end

local function flag(values, key)
    return values[key] == '1' and 'ON' or 'OFF'
end

local function codecName(raw)
    local codecs = {
        [0] = 'UNSETTLED', [1] = 'SBC', [2] = 'AAC', [16] = 'LDAC',
        [32] = 'APT-X', [33] = 'APT-X HD', [48] = 'LC3', [255] = 'OTHER'
    }
    local numeric = tonumber(raw)
    return (numeric and codecs[numeric] or nil) or ('UNKNOWN (' .. tostring(raw or '--') .. ')')
end

local function prettyMode(mode)
    if mode == 'noise_cancelling' then return 'NOISE CANCELLING' end
    if mode == 'ambient' then return 'AMBIENT' end
    if mode == 'off' then return 'OFF' end
    return tostring(mode or '--'):upper()
end

local function setVariable(name, valueToSet)
    SKIN:Bang('!SetVariable', name, tostring(valueToSet or ''))
end

local function appendUnknown(lines, values, known)
    local extra = {}
    for key, _ in pairs(values) do
        if not known[key] then table.insert(extra, key) end
    end
    table.sort(extra)
    for _, key in ipairs(extra) do table.insert(lines, key .. ' = ' .. value(values, key)) end
end

local function makeRawLines(state, settings)
    local lines = {'[State]'}
    local known = {}
    for _, key in ipairs(stateOrder) do
        known[key] = true
        table.insert(lines, key .. ' = ' .. value(state, key))
    end
    appendUnknown(lines, state, known)

    table.insert(lines, '')
    table.insert(lines, '[Bridge configuration]')
    known = {}
    for _, key in ipairs(settingsOrder) do
        known[key] = true
        table.insert(lines, key .. ' = ' .. value(settings, key))
    end
    appendUnknown(lines, settings, known)
    table.insert(lines, 'state_file = ' .. dataPath .. '\\state.ini')
    table.insert(lines, 'settings_file = ' .. settingsPath)
    table.insert(lines, 'log_file = ' .. logPath)
    return lines
end

local function splitLines(lines)
    local midpoint = math.ceil(#lines / 2)
    local left = {}
    local right = {}
    for index, line in ipairs(lines) do
        if index <= midpoint then
            table.insert(left, line)
        else
            table.insert(right, line)
        end
    end
    return table.concat(left, CRLF), table.concat(right, CRLF)
end

local function refreshData()
    local state = readSection(dataPath .. '\\state.ini', 'State')
    local settings = readSection(settingsPath, 'Bridge')
    local logLines = readLogTail(logPath, 8)
    local status = value(state, 'status', 'starting')
    local connected = status == 'connected'
    local recovering = status == 'recovering' or status == 'syncing' or status == 'connecting'

    local statusColor = SKIN:GetVariable('UserMutedColor', '155,155,155,255')
    if connected then statusColor = SKIN:GetVariable('UserTextColor', '245,245,245,255') end
    if status == 'disconnected' then statusColor = SKIN:GetVariable('UserErrorColor', '255,115,115,255') end
    setVariable('DebugStatusColor', statusColor)
    setVariable('DebugDeviceName', value(state, 'device_name', 'WH-1000XM5'))
    setVariable('DebugStatusText', value(state, 'status_text', status):upper())
    setVariable('DebugStatusDetail', value(state, 'error', 'No reported bridge error'))

    local battery = tonumber(state.battery) or 0
    local volume = tonumber(state.volume) or 0
    local connectionSummary = {
        'Status: ' .. status:upper(),
        'Model: ' .. value(state, 'device_name', 'WH-1000XM5'),
        'Address: ' .. value(state, 'device_mac'),
        'Link: ' .. value(state, 'transport') .. ' / ' .. codecName(state.codec),
        'Uptime: ' .. value(state, 'connection_uptime_seconds') .. ' sec',
        'Latency: command ' .. value(state, 'command_latency_ms') .. ' ms / connect ' .. value(state, 'connect_latency_ms') .. ' ms',
        'Attempts: ' .. value(state, 'connection_attempts') .. ' / reconnects ' .. value(state, 'reconnect_count') .. ' / poll errors ' .. value(state, 'poll_error_count'),
        'Bridge: ' .. value(state, 'bridge_version') .. ' / firmware ' .. value(state, 'firmware')
    }
    local audioSummary = {
        'Battery: ' .. tostring(battery) .. '%',
        'Charging: ' .. flag(state, 'charging'),
        'Volume: ' .. tostring(volume) .. ' / 30  (' .. tostring(math.floor(volume / 30 * 100 + 0.5)) .. '%)',
        'Playback: ' .. value(state, 'playback'):upper(),
        'Codec: ' .. codecName(state.codec),
        'Listening: ' .. prettyMode(state.anc_mode),
        'Ambient strength: ' .. value(state, 'ambient_level') .. ' / 20',
        'Track: ' .. value(state, 'track_title')
    }
    local featureSummary = {
        'Speak-to-Chat: ' .. flag(state, 'speak_to_chat'),
        'DSEE: ' .. flag(state, 'dsee'),
        'Wear pause: ' .. flag(state, 'auto_pause'),
        'Touch panel: ' .. flag(state, 'touch_panel'),
        'Multipoint: ' .. flag(state, 'multipoint'),
        'EQ: ' .. value(state, 'eq_preset') .. '  Bass ' .. value(state, 'eq_bass', '0'),
        'Priority: ' .. value(state, 'priority'),
        'Auto off: ' .. value(state, 'auto_off')
    }

    setVariable('DebugConnectionSummary', table.concat(connectionSummary, CRLF))
    setVariable('DebugAudioSummary', table.concat(audioSummary, CRLF))
    setVariable('DebugFeatureSummary', table.concat(featureSummary, CRLF))

    local rawLines = makeRawLines(state, settings)
    local rawLeft, rawRight = splitLines(rawLines)
    setVariable('DebugRawLeft', rawLeft)
    setVariable('DebugRawRight', rawRight)
    setVariable('DebugLog', table.concat(logLines, CRLF))

    local report = {
        'Sony WH-1000XM5 Widget Debug Report',
        'Generated: ' .. os.date('%Y-%m-%d %H:%M:%S'),
        '',
        '[Connected headphone]',
        table.concat(connectionSummary, CRLF),
        '',
        '[Audio]',
        table.concat(audioSummary, CRLF),
        '',
        '[Features]',
        table.concat(featureSummary, CRLF),
        '',
        table.concat(rawLines, CRLF),
        '',
        '[Recent bridge log]',
        table.concat(logLines, CRLF)
    }
    lastReport = table.concat(report, CRLF)
    return battery
end

function Initialize()
    dataPath = SELF:GetOption('DataPath')
    settingsPath = SELF:GetOption('SettingsPath')
    logPath = SELF:GetOption('LogPath')
    refreshData()
end

function Update()
    local battery = refreshData()
    if copyTicks > 0 then
        copyTicks = copyTicks - 1
        if copyTicks == 0 then setVariable('DebugCopyLabel', 'COPY ALL') end
    end
    return battery
end

function Refresh()
    refreshData()
    SKIN:Bang('!UpdateMeter', '*')
    SKIN:Bang('!Redraw')
end

function CopyAll()
    SKIN:Bang('!SetClip', lastReport)
    setVariable('DebugCopyLabel', 'COPIED')
    copyTicks = 4
    SKIN:Bang('!UpdateMeter', 'MeterCopy')
    SKIN:Bang('!Redraw')
end

function Close()
    SKIN:Bang('!DeactivateConfig', 'SonyXM5\\Debug')
end
