local settingsPath
local currentPage = 1
local selectedDesign = 'line'

local function writeValue(name, value)
    value = tostring(value)
    SKIN:Bang('!WriteKeyValue', 'Variables', name, value, settingsPath)
    SKIN:Bang('!SetVariable', name, value)
    SKIN:Bang('!SetVariable', name, value, 'SonyXM5')
end

local function setVariable(name, value)
    SKIN:Bang('!SetVariable', name, tostring(value))
end

local function accentColor()
    return tostring(SKIN:GetVariable('UserSuccessColor', '91,225,145,255'))
end

local function colorWithAlpha(color, alpha)
    local red, green, blue = tostring(color or ''):match('(%d+)%s*,%s*(%d+)%s*,%s*(%d+)')
    if not red then return '0,0,0,' .. tostring(alpha) end
    return table.concat({red, green, blue, tostring(alpha)}, ',')
end

local function setDesignState(prefix, active)
    local accent = accentColor()
    setVariable(prefix .. 'Fill', active and colorWithAlpha(accent, 30) or '30,30,34,248')
    setVariable(prefix .. 'Stroke', active and accent or '76,76,82,180')
    setVariable(prefix .. 'Text', active and accent or '245,245,245,255')
    setVariable(prefix .. 'Badge', active and 'SELECTED' or 'CHOOSE')
end

local function designName()
    if selectedDesign == 'studio' then return 'STUDIO' end
    if selectedDesign == 'mono' then return 'MONO SIGNAL' end
    return 'LINE'
end

local function applyPage()
    local accent = accentColor()
    setVariable('Accent', accent)
    setVariable('AccentSoft', colorWithAlpha(accent, 30))
    for page = 1, 3 do
        SKIN:Bang(page == currentPage and '!ShowMeterGroup' or '!HideMeterGroup', 'WelcomePage' .. tostring(page))
    end
    setVariable('WizardStep', tostring(currentPage) .. ' / 3')
    setVariable('Step1Color', currentPage >= 1 and accent or '76,76,82,180')
    setVariable('Step2Color', currentPage >= 2 and accent or '76,76,82,180')
    setVariable('Step3Color', currentPage >= 3 and accent or '76,76,82,180')
    setDesignState('WizardLine', selectedDesign == 'line')
    setDesignState('WizardStudio', selectedDesign == 'studio')
    setDesignState('WizardMono', selectedDesign == 'mono')
    setVariable('ChosenDesign', designName())
    SKIN:Bang('!UpdateMeter', '*')
    SKIN:Bang('!Redraw')
end

function Initialize()
    settingsPath = SELF:GetOption('SettingsPath')
    selectedDesign = tostring(SKIN:GetVariable('WidgetDesign', 'line')):lower()
    if selectedDesign ~= 'line' and selectedDesign ~= 'studio' and selectedDesign ~= 'mono' then
        selectedDesign = 'line'
    end
    -- A newly opened welcome guide owns the setup flow. Close a stale Settings
    -- panel once here, instead of repeatedly closing it from Update().
    SKIN:Bang('!DeactivateConfig', 'SonyXM5\\Settings')
    applyPage()
end

function Update()
    return 0
end

function Next()
    currentPage = math.min(3, currentPage + 1)
    applyPage()
end

function Back()
    currentPage = math.max(1, currentPage - 1)
    applyPage()
end

function SelectDesign(name)
    name = tostring(name):lower()
    if name == 'line' or name == 'studio' or name == 'mono' then
        selectedDesign = name
        writeValue('WidgetDesign', selectedDesign)
        SKIN:Bang('!CommandMeasure', 'MeasureScript', 'RefreshLayout()', 'SonyXM5')
        applyPage()
    end
end

local function completeSetup()
    writeValue('WidgetDesign', selectedDesign)
    writeValue('WelcomeCompleted', 1)
    SKIN:Bang('!CommandMeasure', 'MeasureScript', 'RefreshLayout()', 'SonyXM5')
end

function Finish()
    completeSetup()
    SKIN:Bang('!DeactivateConfig', 'SonyXM5\\Welcome')
end

function OpenSettings()
    completeSetup()
end

function Skip()
    writeValue('WelcomeCompleted', 1)
    SKIN:Bang('!DeactivateConfig', 'SonyXM5\\Welcome')
end
