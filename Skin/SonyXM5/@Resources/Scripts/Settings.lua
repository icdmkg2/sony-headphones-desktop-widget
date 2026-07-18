local settingsPath
local currentPage = 'appearance'

local defaults = {
    CompactContentWidth = 352,
    CompactSidePadding = 20,
    CompactTopPadding = 14,
    CompactBottomPadding = 14,
    CompactPrimarySize = 10,
    CompactSecondarySize = 7,
    ShowStatusText = 1,
    ShowBatteryIcon = 0,
    ShowButtonIcons = 1,
    EnableAnimations = 1,
    BatteryAlerts = 1,
    BatteryAlertLevel = 20,
    WidgetDesign = 'line',
    ExpandedContentWidth = 392,
    ExpandedSidePadding = 22,
    ExpandedTopPadding = 16,
    ExpandedBottomPadding = 22,
    ExpandedPrimarySize = 10,
    ExpandedSecondarySize = 7,
    ShowAmbientRow = 1,
    ShowTrackArtist = 1,
    ShowQuickControls = 1,
    ShowConnectionHealth = 1,
    UserTextColor = '245,245,245,255',
    UserMutedColor = '155,155,155,255',
    UserLineColor = '92,92,92,180',
    UserDividerColor = '92,92,92,115',
    UserControlFillColor = '34,34,38,225',
    UserDisabledColor = '72,72,72,210',
    UserErrorColor = '255,115,115,255',
    UserSuccessColor = '91,225,145,255'
}

local pages = {'appearance', 'compact', 'expanded', 'behavior', 'tools'}
local accentColors = {
    mint = '91,225,145,255',
    cyan = '84,200,255,255',
    violet = '181,143,255,255',
    amber = '255,190,92,255',
    rose = '255,117,154,255'
}
local accentOverride = nil
local settingsUiCache = {}
local settingsVariableCache = {}

local function number(value, fallback)
    local result = tonumber(value)
    if result == nil then return fallback end
    return result
end

local function writeValue(name, value)
    value = tostring(value)
    SKIN:Bang('!WriteKeyValue', 'Variables', name, value, settingsPath)
    SKIN:Bang('!SetVariable', name, value)
    SKIN:Bang('!SetVariable', name, value, 'SonyXM5')
end

local function setVariable(name, value)
    local text = tostring(value)
    if settingsVariableCache[name] == text then return end
    settingsVariableCache[name] = text
    SKIN:Bang('!SetVariable', name, text)
end

local function colorWithAlpha(color, alpha)
    local red, green, blue = tostring(color or ''):match('(%d+)%s*,%s*(%d+)%s*,%s*(%d+)')
    if not red then return '0,0,0,' .. tostring(alpha) end
    return table.concat({red, green, blue, tostring(alpha)}, ',')
end

local function currentAccent()
    if accentOverride then return accentOverride end
    return tostring(SKIN:GetVariable('UserSuccessColor', defaults.UserSuccessColor))
end

local function redrawWidget()
    SKIN:Bang('!UpdateMeter', '*')
    SKIN:Bang('!Redraw')
    SKIN:Bang('!CommandMeasure', 'MeasureScript', 'RefreshLayout()', 'SonyXM5')
end

local function setButtonState(prefix, active)
    local accent = currentAccent()
    setVariable(prefix .. 'Fill', active and accent or '40,40,44,255')
    setVariable(prefix .. 'Text', active and '17,31,23,255' or '245,245,245,255')
    setVariable(prefix .. 'Stroke', active and accent or '76,76,82,170')
end

local function setOutlineState(prefix, active)
    local accent = currentAccent()
    setVariable(prefix .. 'Fill', active and colorWithAlpha(accent, 28) or '30,30,34,244')
    setVariable(prefix .. 'Text', active and accent or '245,245,245,255')
    setVariable(prefix .. 'Stroke', active and accent or '76,76,82,170')
    setVariable(prefix .. 'State', active and 'ACTIVE' or 'SELECT')
end

local function setAccentState(prefix, color, active)
    setVariable(prefix .. 'Fill', colorWithAlpha(color, active and 255 or 48))
    setVariable(prefix .. 'Text', active and '17,31,23,255' or color)
end

local function applyPage()
    for _, page in ipairs(pages) do
        local group = 'Page' .. page:sub(1, 1):upper() .. page:sub(2)
        SKIN:Bang(page == currentPage and '!ShowMeterGroup' or '!HideMeterGroup', group)
        setButtonState('Nav' .. page:sub(1, 1):upper() .. page:sub(2), page == currentPage)
    end
    SKIN:Bang('!UpdateMeter', '*')
    SKIN:Bang('!Redraw')
end

function Initialize()
    settingsPath = SELF:GetOption('SettingsPath')
    applyPage()
end

function Update()
    local accent = currentAccent()
    local design = tostring(SKIN:GetVariable('WidgetDesign', 'line')):lower()
    if design ~= 'line' and design ~= 'studio' and design ~= 'mono' then design = 'line' end
    local textColor = tostring(SKIN:GetVariable('UserTextColor', defaults.UserTextColor))
    local uiSignature = table.concat({
        accent, design, textColor,
        tostring(number(SKIN:GetVariable('ShowStatusText'), 1)),
        tostring(number(SKIN:GetVariable('ShowBatteryIcon'), 0)),
        tostring(number(SKIN:GetVariable('ShowButtonIcons'), 1)),
        tostring(number(SKIN:GetVariable('EnableAnimations'), 1)),
        tostring(number(SKIN:GetVariable('BatteryAlerts'), 1)),
        tostring(number(SKIN:GetVariable('ShowConnectionHealth'), 1)),
        tostring(number(SKIN:GetVariable('ShowAmbientRow'), 1)),
        tostring(number(SKIN:GetVariable('ShowTrackArtist'), 1)),
        tostring(number(SKIN:GetVariable('ShowQuickControls'), 1))
    }, '|')
    if settingsUiCache.signature == uiSignature then return 0 end
    settingsUiCache.signature = uiSignature

    setVariable('Accent', accent)
    setVariable('AccentSoft', colorWithAlpha(accent, 28))

    local toggles = {
        Status = {'ShowStatusText', 1},
        BatteryIcon = {'ShowBatteryIcon', 0},
        ButtonIcon = {'ShowButtonIcons', 1},
        Animation = {'EnableAnimations', 1},
        BatteryAlert = {'BatteryAlerts', 1},
        Health = {'ShowConnectionHealth', 1},
        Ambient = {'ShowAmbientRow', 1},
        Artist = {'ShowTrackArtist', 1},
        Quick = {'ShowQuickControls', 1}
    }
    for prefix, setting in pairs(toggles) do
        local active = number(SKIN:GetVariable(setting[1]), setting[2]) == 1
        setVariable(prefix .. 'ToggleLabel', active and 'ON' or 'OFF')
        setButtonState(prefix .. 'Toggle', active)
    end

    setOutlineState('DesignLine', design == 'line')
    setOutlineState('DesignStudio', design == 'studio')
    setOutlineState('DesignMono', design == 'mono')

    setButtonState('ThemeWhite', textColor == '245,245,245,255')
    setButtonState('ThemeWarm', textColor == '255,244,226,255')
    setButtonState('ThemeBlue', textColor == '225,238,255,255')
    setButtonState('ThemeDark', textColor == '28,28,32,255')
    setAccentState('AccentMint', accentColors.mint, accent == accentColors.mint)
    setAccentState('AccentCyan', accentColors.cyan, accent == accentColors.cyan)
    setAccentState('AccentViolet', accentColors.violet, accent == accentColors.violet)
    setAccentState('AccentAmber', accentColors.amber, accent == accentColors.amber)
    setAccentState('AccentRose', accentColors.rose, accent == accentColors.rose)
    return 0
end

function Page(name)
    name = tostring(name):lower()
    for _, page in ipairs(pages) do
        if name == page then
            currentPage = name
            applyPage()
            return
        end
    end
end

function Adjust(name, delta, minimum, maximum)
    local current = number(SKIN:GetVariable(name), defaults[name] or 0)
    local value = math.max(number(minimum, current), math.min(number(maximum, current), current + number(delta, 0)))
    settingsUiCache.signature = nil
    writeValue(name, math.floor(value + 0.5))
    redrawWidget()
end

function Toggle(name)
    local value = number(SKIN:GetVariable(name), defaults[name] or 0) == 0 and 1 or 0
    settingsUiCache.signature = nil
    writeValue(name, value)
    redrawWidget()
end

function Theme(name)
    local themes = {
        white = {'245,245,245,255', '155,155,155,255', '92,92,92,180', '92,92,92,115', '34,34,38,225', '72,72,72,210'},
        warm = {'255,244,226,255', '187,169,146,255', '115,101,84,180', '115,101,84,105', '43,38,32,225', '96,84,72,210'},
        blue = {'225,238,255,255', '151,174,203,255', '79,103,132,180', '79,103,132,105', '29,36,47,225', '72,86,104,210'},
        -- Dark ink for light / white wallpapers
        dark = {'28,28,32,255', '102,102,110,255', '70,70,76,170', '70,70,76,100', '238,238,242,235', '168,168,174,210'}
    }
    local colors = themes[name] or themes.white
    settingsUiCache.signature = nil
    writeValue('UserTextColor', colors[1])
    writeValue('UserMutedColor', colors[2])
    writeValue('UserLineColor', colors[3])
    writeValue('UserDividerColor', colors[4])
    writeValue('UserControlFillColor', colors[5])
    writeValue('UserDisabledColor', colors[6])
    redrawWidget()
end

function Accent(name)
    name = tostring(name):lower()
    local color = accentColors[name] or accentColors.mint
    accentOverride = color
    settingsUiCache.signature = nil
    writeValue('UserSuccessColor', color)
    setVariable('Accent', color)
    setVariable('AccentSoft', colorWithAlpha(color, 28))
    Update()
    applyPage()
    redrawWidget()
    accentOverride = nil
end

function Design(name)
    name = tostring(name):lower()
    if name ~= 'line' and name ~= 'studio' and name ~= 'mono' then name = 'line' end
    settingsUiCache.signature = nil
    writeValue('WidgetDesign', name)
    redrawWidget()
end

function Reset()
    settingsUiCache.signature = nil
    for name, value in pairs(defaults) do writeValue(name, value) end
    redrawWidget()
end

function Preview(expanded)
    local value = expanded == true or tostring(expanded):lower() == 'true' or tostring(expanded) == '1'
    SKIN:Bang('!CommandMeasure', 'MeasureScript', value and 'SetDetails(true)' or 'SetDetails(false)', 'SonyXM5')
end

function EditFile()
    SKIN:Bang('"notepad.exe" "' .. settingsPath .. '"')
end

function OpenDebug()
    SKIN:Bang('!ActivateConfig', 'SonyXM5\\Debug', 'Debug.ini')
end

function OpenWelcome()
    SKIN:Bang('!ActivateConfig', 'SonyXM5\\Welcome', 'Welcome.ini')
end

function DockBottom()
    SKIN:Bang('!CommandMeasure', 'MeasureScript', 'DockBottom()', 'SonyXM5')
end

function Close()
    SKIN:Bang('!DeactivateConfig', 'SonyXM5\\Settings')
end
