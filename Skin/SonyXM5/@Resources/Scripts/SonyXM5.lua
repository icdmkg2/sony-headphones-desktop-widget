local dataPath
local queuePath
local bridgePath
local expectedBridgeVersion
local state = {}
local lastGoodState = {}
local optimisticVolume = nil
local optimisticVolumeTicks = 0
local optimisticState = {}
local commandCounter = 0
local bridgeUpgradeActive = false
local bridgeUpgradeTimer = 0
local bridgeUpgradeCommand
local detailsVisible = false
local advancedVisible = false
local lastVisibilitySignature = ''
local powerArmTicks = 0
local currentWidgetWidth = 392
local currentWidgetHeight = 70
local targetWidgetHeight = 70
local animationInitialized = false
local animationPhase = 0
local animatedVolume = nil
local commandPulseTicks = 0
local batteryAlertArmed = true
local lastPositionX = nil
local lastPositionY = nil
local positionStableTicks = 0
local positionClampDelay = 16
local positionClampCooldown = 0
local compactReturnX = nil
local compactReturnY = nil
local compactRestorePending = false
local bridgeLaunchCooldown = 0
local startupStallTicks = 0
local startupRecoveryTimer = 0
local startupRecoveryCommand
local lastLayoutSignature = ''
local variableCache = {}

local COLOR = {
    primary = '245,245,245,255',
    onPrimary = '20,20,20,255',
    primaryContainer = '245,245,245,255',
    onPrimaryContainer = '20,20,20,255',
    surfaceHigh = '45,45,45,230',
    surfaceHighest = '55,55,55,230',
    onSurface = '245,245,245,255',
    onVariant = '155,155,155,255',
    outlineVariant = '76,76,76,210',
    success = '245,245,245,255',
    warning = '145,145,145,255',
    error = '255,115,115,255'
}

local MONO = {
    active = '245,245,245,255',
    inactive = '145,145,145,255',
    disabled = '76,76,76,210',
    line = '95,95,95,180'
}

local function fileExists(path)
    local file = io.open(path, 'rb')
    if file then file:close() return true end
    return false
end

local function readIni(path)
    local values = {}
    local file = io.open(path, 'r')
    if not file then return values end
    local inState = false
    for line in file:lines() do
        line = line:gsub('\r$', '')
        if line:match('^%s*%[') then
            inState = line:lower():match('^%s*%[state%]') ~= nil
        elseif inState and not line:match('^%s*[;#]') then
            local key, value = line:match('^%s*([^=]+)%s*=(.*)$')
            if key then values[key:lower():gsub('%s+$', '')] = value end
        end
    end
    file:close()
    return values
end

local function setVariable(name, value)
    local text = tostring(value or '')
    if variableCache[name] == text then return end
    variableCache[name] = text
    SKIN:Bang('!SetVariable', name, text)
end

local function number(value, fallback)
    local result = tonumber(value)
    if result == nil then return fallback or 0 end
    return result
end

local function settingNumber(name, fallback, minimum, maximum)
    local value = number(SKIN:GetVariable(name), fallback)
    if minimum then value = math.max(minimum, value) end
    if maximum then value = math.min(maximum, value) end
    return value
end

local function settingFlag(name, fallback)
    local value = SKIN:GetVariable(name)
    if value == nil or value == '' then return fallback and 1 or 0 end
    return number(value, fallback and 1 or 0) ~= 0 and 1 or 0
end

local function skinColor(name, fallback)
    local value = SKIN:GetVariable(name)
    if value == nil or value == '' then return fallback end
    return value
end

local function colorWithAlpha(color, alpha)
    local red, green, blue = tostring(color or ''):match('(%d+)%s*,%s*(%d+)%s*,%s*(%d+)')
    if not red then return '0,0,0,' .. tostring(math.floor(alpha + 0.5)) end
    return table.concat({red, green, blue, tostring(math.floor(math.max(0, math.min(255, alpha)) + 0.5))}, ',')
end

local function formatDuration(seconds)
    seconds = math.max(0, math.floor(number(seconds, 0)))
    if seconds < 60 then return tostring(seconds) .. ' SEC' end
    if seconds < 3600 then return tostring(math.floor(seconds / 60)) .. ' MIN' end
    local hours = math.floor(seconds / 3600)
    local minutes = math.floor((seconds % 3600) / 60)
    return string.format('%dH %02dM', hours, minutes)
end

local function codecName(raw)
    local codecs = {
        [0] = 'UNSETTLED', [1] = 'SBC', [2] = 'AAC', [16] = 'LDAC',
        [32] = 'APT-X', [33] = 'APT-X HD', [48] = 'LC3', [255] = 'OTHER'
    }
    local numeric = tonumber(raw)
    return (numeric and codecs[numeric]) or ('CODEC ' .. tostring(raw or '--'))
end

local function updateView(force)
    local prefix = detailsVisible and 'Expanded' or 'Compact'
    local contentWidth = settingNumber(prefix .. 'ContentWidth', detailsVisible and 392 or 352, 300, 560)
    local sidePadding = settingNumber(prefix .. 'SidePadding', 20, 8, 64)
    local topPadding = settingNumber(prefix .. 'TopPadding', 14, 6, 64)
    local bottomPadding = settingNumber(prefix .. 'BottomPadding', 14, 6, 64)
    local primarySize = settingNumber(prefix .. 'PrimarySize', 10, 7, 18)
    local secondarySize = settingNumber(prefix .. 'SecondarySize', 7, 6, 15)
    local showStatus = settingFlag('ShowStatusText', true)
    local showBatteryIcon = settingFlag('ShowBatteryIcon', false)
    local showAmbient = settingFlag('ShowAmbientRow', true)
    local showArtist = settingFlag('ShowTrackArtist', true)
    local showQuick = settingFlag('ShowQuickControls', true)
    local showHealth = settingFlag('ShowConnectionHealth', true)
    local design = tostring(SKIN:GetVariable('WidgetDesign', 'line')):lower()
    local studio = design == 'studio'
    local mono = design == 'mono'
    local spacious = studio or mono
    local batteryLayoutExtra = showBatteryIcon == 1 and 20 or 0
    local layoutSignature = table.concat({
        design, tostring(detailsVisible), tostring(advancedVisible),
        tostring(contentWidth), tostring(sidePadding), tostring(topPadding), tostring(bottomPadding),
        tostring(primarySize), tostring(secondarySize), tostring(showStatus), tostring(showBatteryIcon),
        tostring(showAmbient), tostring(showArtist), tostring(showQuick), tostring(showHealth)
    }, ':')
    if not force and layoutSignature == lastLayoutSignature then return end
    lastLayoutSignature = layoutSignature

    local headerCenter = topPadding + (spacious and 25 or 21)
    local detailTop = topPadding + (spacious and 62 or 50)
    local playbackLine = detailTop + (spacious and (50 + showAmbient * 40) or (44 + showAmbient * 34))
    local trackTitle = playbackLine + (spacious and 30 or (showArtist == 1 and 18 or 27))
    local trackArtist = playbackLine + (spacious and 50 or 37)
    local playbackCenter = playbackLine + (spacious and 36 or 27)
    local volumeCenter = playbackLine + (spacious and 94 or 74)
    local quickLine = playbackLine + (spacious and 122 or 98)
    local quickCenter = quickLine + (spacious and 28 or 21)
    local baseDetailBottom = quickLine + (showQuick == 1 and (spacious and 62 or 42) or (spacious and 18 or 10))
    local healthLine = baseDetailBottom + (spacious and 10 or 8)
    local healthCenter = healthLine + (spacious and 25 or 18)
    local detailBottom = showHealth == 1 and (healthLine + (spacious and 52 or 36)) or baseDetailBottom
    local advancedTop = detailBottom + (spacious and 12 or 10)
    local advancedHeader = advancedTop + 18
    local advancedEq = advancedTop + 54
    local advancedBandRow1 = advancedTop + 94
    local advancedBandRow2 = advancedTop + 132
    local advancedFeatureRow1 = advancedTop + 176
    local advancedFeatureRow2 = advancedTop + 216
    local advancedBottom = advancedTop + 242
    local compactHeight = topPadding + (spacious and 50 or 42) + bottomPadding
    local widgetHeight = detailsVisible and ((advancedVisible and advancedBottom or detailBottom) + bottomPadding) or compactHeight
    local studioQuickWidth = (contentWidth - 24) / 4
    local advancedCellWidth = (contentWidth - 16) / 3
    local advancedHalfWidth = (contentWidth - 8) / 2

    setVariable('CurrentContentWidth', math.floor(contentWidth + 0.5))
    setVariable('CurrentSidePadding', math.floor(sidePadding + 0.5))
    setVariable('CurrentTopPadding', math.floor(topPadding + 0.5))
    setVariable('CurrentBottomPadding', math.floor(bottomPadding + 0.5))
    setVariable('CurrentPrimarySize', math.floor(primarySize + 0.5))
    setVariable('CurrentSecondarySize', math.floor(secondarySize + 0.5))
    setVariable('HeaderCenterY', math.floor(headerCenter + 0.5))
    setVariable('DeviceCenterOffset', showStatus == 1 and -7 or 0)
    setVariable('RuntimeStatusHidden', showStatus == 1 and 0 or 1)
    setVariable('DetailTopY', math.floor(detailTop + 0.5))
    setVariable('PlaybackLineY', math.floor(playbackLine + 0.5))
    setVariable('TrackTitleY', math.floor(trackTitle + 0.5))
    setVariable('TrackArtistY', math.floor(trackArtist + 0.5))
    setVariable('PlaybackCenterY', math.floor(playbackCenter + 0.5))
    setVariable('VolumeCenterY', math.floor(volumeCenter + 0.5))
    setVariable('QuickLineY', math.floor(quickLine + 0.5))
    setVariable('QuickCenterY', math.floor(quickCenter + 0.5))
    setVariable('HealthLineY', math.floor(healthLine + 0.5))
    setVariable('HealthCenterY', math.floor(healthCenter + 0.5))
    setVariable('DetailBottomY', math.floor(detailBottom + 0.5))
    setVariable('AdvancedTopY', math.floor(advancedTop + 0.5))
    setVariable('AdvancedHeaderY', math.floor(advancedHeader + 0.5))
    setVariable('AdvancedEqY', math.floor(advancedEq + 0.5))
    setVariable('AdvancedBandRow1Y', math.floor(advancedBandRow1 + 0.5))
    setVariable('AdvancedBandRow2Y', math.floor(advancedBandRow2 + 0.5))
    setVariable('AdvancedFeatureRow1Y', math.floor(advancedFeatureRow1 + 0.5))
    setVariable('AdvancedFeatureRow2Y', math.floor(advancedFeatureRow2 + 0.5))
    setVariable('AdvancedBottomY', math.floor(advancedBottom + 0.5))
    setVariable('VolumeRailWidth', math.floor(contentWidth - 115 + 0.5))
    setVariable('StudioVolumeRailWidth', math.floor(contentWidth - 205 + 0.5))
    setVariable('BatteryLayoutExtra', batteryLayoutExtra)
    setVariable('ClassicDeviceWidth', math.floor(contentWidth - (detailsVisible and 246 or 208) - batteryLayoutExtra + 0.5))
    setVariable('ClassicAncRight', detailsVisible and 100 or 64)
    setVariable('ClassicAncWidth', detailsVisible and 56 or 54)
    setVariable('ClassicExpandRight', detailsVisible and 52 or 18)
    setVariable('ClassicExpandWidth', detailsVisible and 32 or 36)
    setVariable('StudioDeviceWidth', math.floor(contentWidth - (detailsVisible and 192 or 238) - batteryLayoutExtra + 0.5))
    setVariable('SignalDeviceWidth', math.floor(contentWidth - (detailsVisible and 196 or 184) - batteryLayoutExtra + 0.5))
    setVariable('SignalVolumeRailWidth', math.floor(contentWidth - 172 + 0.5))
    setVariable('StudioQuickWidth', math.floor(studioQuickWidth + 0.5))
    for index = 0, 3 do
        local left = sidePadding + index * (studioQuickWidth + 8)
        setVariable('StudioQuick' .. tostring(index + 1) .. 'X', math.floor(left + 0.5))
        setVariable('StudioQuick' .. tostring(index + 1) .. 'Center', math.floor(left + studioQuickWidth / 2 + 0.5))
    end
    setVariable('AdvancedCellWidth', math.floor(advancedCellWidth + 0.5))
    for index = 0, 2 do
        local left = sidePadding + index * (advancedCellWidth + 8)
        setVariable('AdvancedCell' .. tostring(index + 1) .. 'X', math.floor(left + 0.5))
        setVariable('AdvancedCell' .. tostring(index + 1) .. 'MinusX', math.floor(left + advancedCellWidth - 34 + 0.5))
        setVariable('AdvancedCell' .. tostring(index + 1) .. 'PlusX', math.floor(left + advancedCellWidth - 10 + 0.5))
    end
    setVariable('AdvancedHalfWidth', math.floor(advancedHalfWidth + 0.5))
    setVariable('AdvancedHalf1X', math.floor(sidePadding + 0.5))
    setVariable('AdvancedHalf1Center', math.floor(sidePadding + advancedHalfWidth / 2 + 0.5))
    setVariable('AdvancedHalf2X', math.floor(sidePadding + advancedHalfWidth + 8 + 0.5))
    setVariable('AdvancedHalf2Center', math.floor(sidePadding + advancedHalfWidth + 8 + advancedHalfWidth / 2 + 0.5))
    setVariable('AdvancedEqBoxX', math.floor(sidePadding + contentWidth - 204 + 0.5))
    setVariable('AdvancedEqPreviousX', math.floor(sidePadding + contentWidth - 182 + 0.5))
    setVariable('AdvancedEqValueX', math.floor(sidePadding + contentWidth - 102 + 0.5))
    setVariable('AdvancedEqNextX', math.floor(sidePadding + contentWidth - 22 + 0.5))
    currentWidgetWidth = math.floor(contentWidth + sidePadding * 2 + 0.5)
    targetWidgetHeight = math.floor(widgetHeight + 0.5)
    if not animationInitialized or settingFlag('EnableAnimations', true) == 0 then
        currentWidgetHeight = targetWidgetHeight
    end
    setVariable('WidgetWidth', currentWidgetWidth)
    setVariable('WidgetHeight', math.floor(currentWidgetHeight + 0.5))
    setVariable('ExpandIcon', detailsVisible and '-' or '+')
    setVariable('ExpandLabel', detailsVisible and 'LESS' or 'MORE')
    setVariable('ExpandChevronEdgeY', detailsVisible and 19 or 11)
    setVariable('ExpandChevronCenterY', detailsVisible and 13 or 17)
    setVariable('AdvancedLabel', advancedVisible and 'BASIC' or 'ADVANCED')
end

local function updateVisibility(force)
    local showStatus = settingFlag('ShowStatusText', true) == 1
    local showBatteryIcon = settingFlag('ShowBatteryIcon', false) == 1
    local showButtonIcons = settingFlag('ShowButtonIcons', true) == 1
    local showAmbient = settingFlag('ShowAmbientRow', true) == 1
    local showArtist = settingFlag('ShowTrackArtist', true) == 1
    local showQuick = settingFlag('ShowQuickControls', true) == 1
    local showHealth = settingFlag('ShowConnectionHealth', true) == 1
    local design = tostring(SKIN:GetVariable('WidgetDesign', 'line')):lower()
    local studio = design == 'studio'
    local mono = design == 'mono'
    local signature = table.concat({design, tostring(detailsVisible), tostring(advancedVisible), tostring(showStatus), tostring(showBatteryIcon), tostring(showButtonIcons), tostring(showAmbient), tostring(showArtist), tostring(showQuick), tostring(showHealth)}, ':')
    if not force and signature == lastVisibilitySignature then return end
    lastVisibilitySignature = signature

    SKIN:Bang('!HideMeterGroup', 'SignalHeader')
    SKIN:Bang('!HideMeterGroup', 'SignalDetails')

    if mono then
        SKIN:Bang('!HideMeterGroup', 'ClassicHeader')
        SKIN:Bang('!HideMeterGroup', 'ClassicBatteryIcon')
        SKIN:Bang('!HideMeterGroup', 'ClassicExpandedAction')
        SKIN:Bang('!HideMeterGroup', 'ClassicSettingsIcon')
        SKIN:Bang('!HideMeterGroup', 'ClassicButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'ClassicButtonText')
        SKIN:Bang('!HideMeterGroup', 'ClassicDetailButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'ClassicDetailButtonText')
        SKIN:Bang('!HideMeterGroup', 'Details')
        SKIN:Bang('!HideMeterGroup', 'StudioHeader')
        SKIN:Bang('!HideMeterGroup', 'StudioBatteryIcon')
        SKIN:Bang('!HideMeterGroup', 'StudioCompactActions')
        SKIN:Bang('!HideMeterGroup', 'StudioExpandedAction')
        SKIN:Bang('!HideMeterGroup', 'StudioSettingsIcon')
        SKIN:Bang('!HideMeterGroup', 'StudioCompactButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'StudioExpandedButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'StudioDetailButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'StudioDetails')
        SKIN:Bang('!ShowMeterGroup', 'SignalHeader')
        SKIN:Bang(showBatteryIcon and '!ShowMeterGroup' or '!HideMeterGroup', 'SignalBatteryIcon')
        SKIN:Bang(showStatus and '!ShowMeter' or '!HideMeter', 'MeterSignalStatus')
        if detailsVisible then
            SKIN:Bang('!HideMeterGroup', 'SignalCompactActions')
            SKIN:Bang('!ShowMeterGroup', 'SignalExpandedAction')
            SKIN:Bang('!ShowMeterGroup', 'SignalDetails')
            SKIN:Bang(showAmbient and '!ShowMeterGroup' or '!HideMeterGroup', 'SignalAmbientOptions')
            SKIN:Bang(showArtist and '!ShowMeterGroup' or '!HideMeterGroup', 'SignalArtistOption')
            SKIN:Bang(showQuick and '!ShowMeterGroup' or '!HideMeterGroup', 'SignalQuickOptions')
            SKIN:Bang(showHealth and '!ShowMeterGroup' or '!HideMeterGroup', 'SignalHealthOptions')
            SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'SignalDetailButtonText')
            SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'SignalDetailButtonIcons')
            SKIN:Bang(advancedVisible and '!ShowMeterGroup' or '!HideMeterGroup', 'AdvancedLine')
            SKIN:Bang('!HideMeterGroup', 'AdvancedStudio')
        else
            SKIN:Bang('!ShowMeterGroup', 'SignalCompactActions')
            SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'SignalCompactButtonText')
            SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'SignalCompactButtonIcons')
            SKIN:Bang('!HideMeterGroup', 'SignalExpandedAction')
            SKIN:Bang('!HideMeterGroup', 'SignalDetails')
            SKIN:Bang('!HideMeterGroup', 'AdvancedLine')
            SKIN:Bang('!HideMeterGroup', 'AdvancedStudio')
        end
    elseif studio then
        SKIN:Bang('!HideMeterGroup', 'ClassicHeader')
        SKIN:Bang('!HideMeterGroup', 'ClassicBatteryIcon')
        SKIN:Bang('!HideMeterGroup', 'ClassicExpandedAction')
        SKIN:Bang('!HideMeterGroup', 'ClassicSettingsIcon')
        SKIN:Bang('!HideMeterGroup', 'ClassicButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'ClassicButtonText')
        SKIN:Bang('!HideMeterGroup', 'ClassicDetailButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'ClassicDetailButtonText')
        SKIN:Bang('!HideMeterGroup', 'Details')
        SKIN:Bang('!ShowMeterGroup', 'StudioHeader')
        SKIN:Bang(showBatteryIcon and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioBatteryIcon')
        SKIN:Bang(showStatus and '!ShowMeter' or '!HideMeter', 'MeterStudioStatus')
        if detailsVisible then
            SKIN:Bang('!HideMeterGroup', 'StudioCompactActions')
            SKIN:Bang('!ShowMeterGroup', 'StudioExpandedAction')
            SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'StudioSettingsText')
            SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioSettingsIcon')
            SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'StudioExpandedButtonText')
            SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioExpandedButtonIcons')
            SKIN:Bang('!ShowMeterGroup', 'StudioDetails')
            SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'StudioDetailButtonText')
            SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioDetailButtonIcons')
            SKIN:Bang(showAmbient and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioAmbientOptions')
            SKIN:Bang(showArtist and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioArtistOption')
            SKIN:Bang(showQuick and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioQuickOptions')
            SKIN:Bang(showHealth and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioHealthOptions')
            SKIN:Bang(advancedVisible and '!ShowMeterGroup' or '!HideMeterGroup', 'AdvancedStudio')
            SKIN:Bang('!HideMeterGroup', 'AdvancedLine')
        else
            SKIN:Bang('!ShowMeterGroup', 'StudioCompactActions')
            SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'StudioCompactButtonText')
            SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'StudioCompactButtonIcons')
            SKIN:Bang('!HideMeterGroup', 'StudioExpandedAction')
            SKIN:Bang('!HideMeterGroup', 'StudioSettingsIcon')
            SKIN:Bang('!HideMeterGroup', 'StudioExpandedButtonIcons')
            SKIN:Bang('!HideMeterGroup', 'StudioDetails')
            SKIN:Bang('!HideMeterGroup', 'StudioHealthOptions')
            SKIN:Bang('!HideMeterGroup', 'AdvancedStudio')
            SKIN:Bang('!HideMeterGroup', 'AdvancedLine')
        end
    else
        SKIN:Bang('!HideMeterGroup', 'StudioHeader')
        SKIN:Bang('!HideMeterGroup', 'StudioBatteryIcon')
        SKIN:Bang('!HideMeterGroup', 'StudioCompactActions')
        SKIN:Bang('!HideMeterGroup', 'StudioExpandedAction')
        SKIN:Bang('!HideMeterGroup', 'StudioSettingsIcon')
        SKIN:Bang('!HideMeterGroup', 'StudioCompactButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'StudioExpandedButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'StudioDetailButtonIcons')
        SKIN:Bang('!HideMeterGroup', 'StudioDetails')
        SKIN:Bang('!ShowMeterGroup', 'ClassicHeader')
        SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'ClassicButtonText')
        SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'ClassicButtonIcons')
        SKIN:Bang(showBatteryIcon and '!ShowMeterGroup' or '!HideMeterGroup', 'ClassicBatteryIcon')
        SKIN:Bang(detailsVisible and '!ShowMeterGroup' or '!HideMeterGroup', 'ClassicExpandedAction')
        SKIN:Bang(showStatus and '!ShowMeter' or '!HideMeter', 'MeterStatus')
        if detailsVisible then
            SKIN:Bang('!ShowMeterGroup', 'Details')
            SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'ClassicSettingsText')
            SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'ClassicSettingsIcon')
            SKIN:Bang(showButtonIcons and '!HideMeterGroup' or '!ShowMeterGroup', 'ClassicDetailButtonText')
            SKIN:Bang(showButtonIcons and '!ShowMeterGroup' or '!HideMeterGroup', 'ClassicDetailButtonIcons')
            SKIN:Bang(showAmbient and '!ShowMeterGroup' or '!HideMeterGroup', 'AmbientOptions')
            SKIN:Bang(showArtist and '!ShowMeterGroup' or '!HideMeterGroup', 'TrackArtistOption')
            SKIN:Bang(showQuick and '!ShowMeterGroup' or '!HideMeterGroup', 'QuickOptions')
            SKIN:Bang(showHealth and '!ShowMeterGroup' or '!HideMeterGroup', 'HealthOptions')
            SKIN:Bang(advancedVisible and '!ShowMeterGroup' or '!HideMeterGroup', 'AdvancedLine')
            SKIN:Bang('!HideMeterGroup', 'AdvancedStudio')
        else
            SKIN:Bang('!HideMeterGroup', 'Details')
            SKIN:Bang('!HideMeterGroup', 'ClassicSettingsIcon')
            SKIN:Bang('!HideMeterGroup', 'ClassicDetailButtonIcons')
            SKIN:Bang('!HideMeterGroup', 'HealthOptions')
            SKIN:Bang('!HideMeterGroup', 'AdvancedLine')
            SKIN:Bang('!HideMeterGroup', 'AdvancedStudio')
        end
    end
end

local function isOn(key)
    return state[key] == '1'
end

local function toggleColors(key, supportedKey)
    local supported = supportedKey == nil or state[supportedKey] == '1'
    if not supported then return COLOR.surfaceHigh, COLOR.outlineVariant end
    if isOn(key) then return skinColor('UserSuccessColor', '91,225,145,255'), '17,31,23,255' end
    return COLOR.surfaceHigh, COLOR.onVariant
end

local function applyToggle(variablePrefix, key, supportedKey)
    local fill, text = toggleColors(key, supportedKey)
    setVariable(variablePrefix .. 'Fill', fill)
    setVariable(variablePrefix .. 'Text', text)
end

local function applyState()
    local status = state.status or 'setup'
    local connected = status == 'connected' or status == 'syncing' or status == 'recovering'
    local battery = math.max(0, math.min(100, number(state.battery, 0)))
    local volume = math.max(0, math.min(30, number(state.volume, 0)))
    local mono = {
        active = skinColor('UserSuccessColor', '91,225,145,255'),
        inactive = skinColor('UserMutedColor', MONO.inactive),
        disabled = skinColor('UserDisabledColor', MONO.disabled),
        line = skinColor('UserDividerColor', MONO.line),
        error = skinColor('UserErrorColor', COLOR.error)
    }
    local statusColor = mono.inactive
    if connected then statusColor = mono.active end
    if status == 'recovering' then statusColor = mono.inactive end
    if status == 'disconnected' then statusColor = mono.error end
    if status == 'setup' or status == 'stopped' then statusColor = mono.disabled end

    setVariable('DeviceName', state.device_name ~= '' and state.device_name or 'WH-1000XM5')
    setVariable('StatusText', state.status_text or 'Starting bridge')
    setVariable('StatusColor', statusColor)
    setVariable('StatusDetail', state.error or '')
    setVariable('ErrorText', state.error or '')
    setVariable('Battery', battery)
    local batteryThreshold = settingNumber('BatteryAlertLevel', 20, 5, 50)
    local batteryLow = connected and not isOn('charging') and battery <= batteryThreshold
    setVariable('BatteryColor', batteryLow and mono.error or mono.active)
    setVariable('BatteryIconFillWidth', connected and math.max(0.1, battery * 0.1) or 0.1)
    setVariable('SignalBatteryFill', connected and math.floor(battery * 0.52 + 0.5) or 0)
    setVariable('BatteryDisplay', connected and battery or '--')
    setVariable('BatteryCompact', connected and (tostring(battery) .. '%') or '--')
    local batteryCaption = isOn('charging') and 'Charging now' or (connected and 'Headphone battery' or 'Waiting for headphones')
    if status == 'recovering' then batteryCaption = 'Last known battery - reconnecting' end
    setVariable('BatteryCaption', batteryCaption)
    setVariable('Volume', volume)
    if animatedVolume == nil or settingFlag('EnableAnimations', true) == 0 then
        animatedVolume = volume
    else
        local difference = volume - animatedVolume
        animatedVolume = math.abs(difference) < 0.04 and volume or (animatedVolume + difference * 0.38)
    end
    setVariable('VolumePercent', math.floor((volume / 30) * 100 + 0.5))
    setVariable('VolumeFill', math.floor((animatedVolume / 30) * 260 + 0.5))
    setVariable('MiniVolumeFill', math.floor((animatedVolume / 30) * 154 + 0.5))
    setVariable('DetailVolumeFill', math.floor((animatedVolume / 30) * settingNumber('VolumeRailWidth', 237, 100, 445) + 0.5))
    setVariable('StudioVolumeFill', math.floor((animatedVolume / 30) * settingNumber('StudioVolumeRailWidth', 187, 80, 355) + 0.5))
    setVariable('SignalVolumeFill', math.floor((animatedVolume / 30) * settingNumber('SignalVolumeRailWidth', 220, 80, 400) + 0.5))
    setVariable('PlaybackIcon', state.playback == 'playing' and '||' or '>')
    setVariable('PlaybackLabel', state.playback == 'playing' and 'PAUSE' or 'PLAY')
    setVariable('TrackTitle', state.track_title ~= '' and state.track_title or 'Windows audio')
    setVariable('TrackArtist', state.track_artist ~= '' and state.track_artist or 'Playback controls')
    setVariable('AmbientLevel', number(state.ambient_level, 20))

    local anc = state.anc_mode or 'off'
    local function modeColors(active)
        if active then return mono.active, '17,31,23,255' end
        return COLOR.surfaceHighest, COLOR.onVariant
    end
    local fill, text = modeColors(anc == 'noise_cancelling')
    setVariable('AncNoiseFill', fill) setVariable('AncNoiseText', text)
    fill, text = modeColors(anc == 'ambient')
    setVariable('AncAmbientFill', fill) setVariable('AncAmbientText', text)
    fill, text = modeColors(anc == 'off')
    setVariable('AncOffFill', fill) setVariable('AncOffText', text)

    local miniAncLabel = 'OFF'
    if anc == 'noise_cancelling' then miniAncLabel = 'NC' end
    if anc == 'ambient' then miniAncLabel = 'AMBIENT' end
    local miniAncSupported = state.supported_anc == '1' or state.supported_ambient == '1'
    if miniAncSupported then
        fill, text = modeColors(true)
    else
        fill, text = COLOR.surfaceHigh, COLOR.outlineVariant
    end
    setVariable('MiniAncLabel', miniAncLabel)
    setVariable('MiniAncFill', fill)
    setVariable('MiniAncText', text)
    setVariable('MonoAncCurrent', miniAncSupported and mono.active or mono.disabled)
    local compactIconColor = miniAncSupported and mono.active or mono.disabled
    local transparent = '0,0,0,0'
    setVariable('CompactAncNoiseIconColor', anc == 'noise_cancelling' and compactIconColor or transparent)
    setVariable('CompactAncAmbientIconColor', anc == 'ambient' and compactIconColor or transparent)
    setVariable('CompactAncOffIconColor', anc == 'off' and compactIconColor or transparent)
    setVariable('MonoNoiseColor', miniAncSupported and (anc == 'noise_cancelling' and mono.active or mono.inactive) or mono.disabled)
    setVariable('MonoAmbientColor', state.supported_ambient == '1' and (anc == 'ambient' and mono.active or mono.inactive) or mono.disabled)
    setVariable('MonoOffColor', miniAncSupported and (anc == 'off' and mono.active or mono.inactive) or mono.disabled)

    local level = number(state.ambient_level, 0)
    for _, threshold in ipairs({1, 5, 10, 15, 20}) do
        setVariable('AmbDot' .. threshold, level >= threshold and mono.active or COLOR.outlineVariant)
        setVariable('AmbMono' .. threshold, level >= threshold and mono.active or mono.line)
    end

    applyToggle('ChipFocus', 'focus_voice', 'supported_ambient')
    applyToggle('ChipSpeak', 'speak_to_chat', 'supported_speak_to_chat')
    applyToggle('ChipDsee', 'dsee', nil)
    applyToggle('ChipPause', 'auto_pause', 'supported_auto_pause')
    applyToggle('ChipTouch', 'touch_panel', 'supported_touch_panel')
    applyToggle('ChipMulti', 'multipoint', 'supported_multipoint')

    local eqSupported = state.supported_eq == '1'
    local multiSupported = state.supported_multipoint == '1'
    local powerSupported = state.supported_power_off == '1'
    setVariable('AdvancedEqColor', eqSupported and mono.active or mono.disabled)
    setVariable('AdvancedMultiColor', multiSupported and (isOn('multipoint') and mono.active or mono.inactive) or mono.disabled)
    setVariable('AdvancedPowerColor', powerSupported and mono.error or mono.disabled)

    local priority = state.priority or '--'
    local priorityShort = priority
    if priority == 'Sound quality' then priorityShort = 'QUALITY' end
    if priority == 'Stable connection' then priorityShort = 'STABLE' end
    if priority == 'Low latency' then priorityShort = 'LOW LATENCY' end
    local autoOff = state.auto_off or '--'
    autoOff = autoOff:gsub(' minutes', ' MIN')
    setVariable('AdvancedMultiLabel', 'MULTIPOINT  ' .. (isOn('multipoint') and 'ON' or 'OFF'))
    setVariable('AdvancedPriorityLabel', 'PRIORITY  ' .. priorityShort:upper())
    setVariable('AdvancedAutoOffLabel', 'AUTO OFF  ' .. autoOff:upper())
    setVariable('AdvancedPowerLabel', powerArmTicks > 0 and 'CLICK AGAIN TO POWER OFF' or 'POWER OFF')

    local function monoToggle(key, supportedKey)
        if supportedKey and state[supportedKey] ~= '1' then return mono.disabled end
        return isOn(key) and mono.active or mono.inactive
    end
    setVariable('MonoSpeakColor', monoToggle('speak_to_chat', 'supported_speak_to_chat'))
    setVariable('MonoDseeColor', monoToggle('dsee', nil))
    setVariable('MonoPauseColor', monoToggle('auto_pause', 'supported_auto_pause'))

    setVariable('EqPreset', state.eq_preset or '--')
    setVariable('EqBass', number(state.eq_bass, 0))
    for index = 1, 5 do setVariable('EqBand' .. index, number(state['eq_band_' .. index], 0)) end
    setVariable('Priority', priority)
    setVariable('AutoOff', state.auto_off or '--')
    setVariable('ButtonFunction', state.button_function or '--')
    setVariable('TouchLeft', state.touch_left or '--')
    setVariable('TouchRight', state.touch_right or '--')
    local firmware = state.firmware or '--'
    local codec = codecName(state.codec)
    setVariable('Firmware', firmware)
    setVariable('Codec', codec)
    setVariable('AdvancedInfo', codec .. '   /   FW ' .. firmware)

    local healthLabel = 'STARTING'
    if status == 'connected' then healthLabel = 'HEALTHY' end
    if status == 'connecting' or status == 'searching' or status == 'syncing' or status == 'updating' then healthLabel = 'CONNECTING' end
    if status == 'recovering' then healthLabel = 'RECOVERING' end
    if status == 'disconnected' then healthLabel = 'OFFLINE' end
    if status == 'setup' or status == 'stopped' then healthLabel = 'INACTIVE' end
    local healthColor = mono.inactive
    if status == 'connected' then healthColor = mono.active end
    if status == 'disconnected' then healthColor = mono.error end
    if status == 'setup' or status == 'stopped' then healthColor = mono.disabled end
    local transport = tostring(state.transport or '--'):upper()
    local commandLatency = number(state.command_latency_ms, -1)
    local commandLatencyText = commandLatency >= 0 and (tostring(math.floor(commandLatency + 0.5)) .. ' MS') or '-- MS'
    local uptimeText = formatDuration(state.connection_uptime_seconds)
    local connectLatency = number(state.connect_latency_ms, -1)
    local connectLatencyText = connectLatency >= 0 and (tostring(math.floor(connectLatency + 0.5)) .. ' ms') or '--'
    local lastDisconnect = state.last_disconnect
    if not lastDisconnect or lastDisconnect == '' then lastDisconnect = 'None recorded' end
    setVariable('HealthLabel', healthLabel)
    setVariable('HealthColor', healthColor)
    setVariable('HealthTransport', transport)
    setVariable('HealthCodec', codec)
    setVariable('HealthLatency', commandLatencyText)
    setVariable('HealthUptime', uptimeText)
    setVariable('HealthSummary', transport .. ' / ' .. codec .. ' / CMD ' .. commandLatencyText .. ' / ' .. uptimeText)
    setVariable('HealthDetail', 'Connect: ' .. connectLatencyText
        .. ' | Attempts: ' .. tostring(math.floor(number(state.connection_attempts, 0)))
        .. ' | Reconnects: ' .. tostring(math.floor(number(state.reconnect_count, 0)))
        .. ' | Poll errors: ' .. tostring(math.floor(number(state.poll_error_count, 0)))
        .. ' | Last disconnect: ' .. lastDisconnect)
end

local function triggerBatteryAlert(battery)
    local value = math.max(0, math.min(100, math.floor(number(battery, 20) + 0.5)))
    setVariable('BatteryAlertValue', value)
    SKIN:Bang('!UpdateMeasure', 'MeasureBatteryAlert')
    SKIN:Bang('!CommandMeasure', 'MeasureBatteryAlert', 'Run')
end

local function updateBatteryAlert()
    local battery = math.max(0, math.min(100, number(state.battery, 0)))
    local threshold = settingNumber('BatteryAlertLevel', 20, 5, 50)
    local connected = state.status == 'connected'
    local charging = isOn('charging')
    if charging or battery > threshold + 5 then batteryAlertArmed = true end
    if settingFlag('BatteryAlerts', true) == 0 then return end
    if connected and not charging and battery <= threshold and batteryAlertArmed then
        batteryAlertArmed = false
        triggerBatteryAlert(battery)
    end
end

local function updateAnimations()
    local enabled = settingFlag('EnableAnimations', true) == 1
    animationPhase = (animationPhase + 0.34) % (math.pi * 2)
    local wave = (math.sin(animationPhase) + 1) * 0.5

    if not enabled then
        currentWidgetHeight = targetWidgetHeight
    else
        local difference = targetWidgetHeight - currentWidgetHeight
        if math.abs(difference) < 0.7 then
            currentWidgetHeight = targetWidgetHeight
        else
            currentWidgetHeight = currentWidgetHeight + difference * 0.30
        end
    end
    setVariable('WidgetHeight', math.floor(currentWidgetHeight + 0.5))

    local status = state.status or 'starting'
    local pulseColor = skinColor('UserMutedColor', MONO.inactive)
    local pulseAlpha = 0
    local pulseRadius = 3.5
    local isTransitioning = status == 'starting' or status == 'searching' or status == 'connecting'
        or status == 'syncing' or status == 'recovering' or status == 'updating'
    if enabled and commandPulseTicks > 0 then
        pulseColor = skinColor('UserTextColor', MONO.active)
        pulseAlpha = 45 + 125 * (commandPulseTicks / 12)
        pulseRadius = 3.5 + 2.5 * (1 - commandPulseTicks / 12)
    elseif enabled and isTransitioning then
        pulseAlpha = 35 + 95 * wave
        pulseRadius = 3.5 + 2.2 * wave
    elseif enabled and status == 'disconnected' then
        pulseColor = skinColor('UserErrorColor', COLOR.error)
        pulseAlpha = 30 + 70 * wave
        pulseRadius = 3.5 + 1.5 * wave
    end
    setVariable('StatusPulseColor', colorWithAlpha(pulseColor, pulseAlpha))
    setVariable('StatusPulseRadius', string.format('%.2f', pulseRadius))
    if commandPulseTicks > 0 then commandPulseTicks = commandPulseTicks - 1 end

    local battery = math.max(0, math.min(100, number(state.battery, 0)))
    local threshold = settingNumber('BatteryAlertLevel', 20, 5, 50)
    local low = (state.status == 'connected' or state.status == 'recovering') and not isOn('charging') and battery <= threshold
    if enabled and low then
        setVariable('BatteryColor', colorWithAlpha(skinColor('UserErrorColor', COLOR.error), 205 + 50 * wave))
    end
end

local function preserveStateDuringRecovery()
    local status = state.status or ''
    if status == 'connected' then
        lastGoodState = {}
        for key, value in pairs(state) do lastGoodState[key] = value end
        return
    end

    if status ~= 'recovering' and status ~= 'syncing' and status ~= 'connecting' then return end
    if next(lastGoodState) == nil then return end

    for key, value in pairs(lastGoodState) do
        if key ~= 'status' and key ~= 'status_text' and key ~= 'error'
            and key ~= 'bridge_version' and key ~= 'last_command' then
            state[key] = value
        end
    end
end

local function preserveOptimisticVolume()
    if optimisticVolume == nil then return end
    local actual = math.max(0, math.min(30, number(state.volume, 0)))
    if actual == optimisticVolume then
        optimisticVolume = nil
        optimisticVolumeTicks = 0
    elseif optimisticVolumeTicks > 0 then
        state.volume = tostring(optimisticVolume)
        optimisticVolumeTicks = optimisticVolumeTicks - 1
    else
        optimisticVolume = nil
    end
end

local function preserveOptimisticState()
    for key, pending in pairs(optimisticState) do
        if state[key] == pending.value then
            optimisticState[key] = nil
        elseif pending.ticks > 0 then
            state[key] = pending.value
            pending.ticks = pending.ticks - 1
        else
            optimisticState[key] = nil
        end
    end
end

local function setOptimisticState(key, value)
    value = tostring(value)
    state[key] = value
    optimisticState[key] = {value = value, ticks = 30}
end

local function renderImmediate()
    applyState()
    SKIN:Bang('!UpdateMeter', '*')
    SKIN:Bang('!Redraw')
end

local function schedulePositionClamp()
    positionClampDelay = 16
end

local function captureCompactPosition()
    local x = tonumber(SKIN:GetVariable('CURRENTCONFIGX'))
    local y = tonumber(SKIN:GetVariable('CURRENTCONFIGY'))
    if not x or not y then return end
    compactReturnX = math.floor(x)
    compactReturnY = math.floor(y)
    compactRestorePending = false
end

local function restoreCompactPosition()
    if compactReturnX == nil or compactReturnY == nil then
        compactRestorePending = false
        return
    end

    local targetX = compactReturnX
    local targetY = compactReturnY
    local workX = tonumber(SKIN:GetVariable('WORKAREAX'))
    local workY = tonumber(SKIN:GetVariable('WORKAREAY'))
    local workWidth = tonumber(SKIN:GetVariable('WORKAREAWIDTH'))
    local workHeight = tonumber(SKIN:GetVariable('WORKAREAHEIGHT'))
    if workX and workY and workWidth and workHeight and workWidth > 0 and workHeight > 0 then
        local maximumX = math.max(workX, workX + workWidth - currentWidgetWidth)
        local maximumY = math.max(workY, workY + workHeight - targetWidgetHeight)
        targetX = math.floor(math.max(workX, math.min(maximumX, targetX)))
        targetY = math.floor(math.max(workY, math.min(maximumY, targetY)))
    end

    SKIN:Bang('!SetWindowPosition', tostring(targetX), tostring(targetY))
    lastPositionX = targetX
    lastPositionY = targetY
    positionStableTicks = 0
    positionClampDelay = 0
    positionClampCooldown = 16
    compactReturnX = nil
    compactReturnY = nil
    compactRestorePending = false
end

local function clampExpandedToWorkArea()
    local x = tonumber(SKIN:GetVariable('CURRENTCONFIGX'))
    local y = tonumber(SKIN:GetVariable('CURRENTCONFIGY'))
    local workX = tonumber(SKIN:GetVariable('WORKAREAX'))
    local workY = tonumber(SKIN:GetVariable('WORKAREAY'))
    local workWidth = tonumber(SKIN:GetVariable('WORKAREAWIDTH'))
    local workHeight = tonumber(SKIN:GetVariable('WORKAREAHEIGHT'))
    if not x or not y or not workX or not workY or not workWidth or not workHeight then return end
    if workWidth <= 0 or workHeight <= 0 then return end

    local maximumX = math.max(workX, workX + workWidth - currentWidgetWidth)
    local maximumY = math.max(workY, workY + workHeight - targetWidgetHeight)
    local targetX = math.floor(math.max(workX, math.min(maximumX, x)))
    local targetY = math.floor(math.max(workY, math.min(maximumY, y)))
    SKIN:Bang('!SetWindowPosition', tostring(targetX), tostring(targetY))
    lastPositionX = targetX
    lastPositionY = targetY
    positionStableTicks = 0
    positionClampDelay = 0
    positionClampCooldown = 16
end

local function clampToWorkArea()
    local x = tonumber(SKIN:GetVariable('CURRENTCONFIGX'))
    local y = tonumber(SKIN:GetVariable('CURRENTCONFIGY'))
    local workX = tonumber(SKIN:GetVariable('WORKAREAX'))
    local workY = tonumber(SKIN:GetVariable('WORKAREAY'))
    local workWidth = tonumber(SKIN:GetVariable('WORKAREAWIDTH'))
    local workHeight = tonumber(SKIN:GetVariable('WORKAREAHEIGHT'))
    if not x or not y or not workX or not workY or not workWidth or not workHeight then return end
    if workWidth <= 0 or workHeight <= 0 then return end

    local renderedWidth = tonumber(SKIN:GetVariable('CURRENTCONFIGWIDTH')) or currentWidgetWidth
    local renderedHeight = tonumber(SKIN:GetVariable('CURRENTCONFIGHEIGHT')) or currentWidgetHeight
    if renderedWidth <= 0 then renderedWidth = currentWidgetWidth end
    if renderedHeight <= 0 then renderedHeight = currentWidgetHeight end

    local maximumX = math.max(workX, workX + workWidth - renderedWidth)
    local maximumY = math.max(workY, workY + workHeight - renderedHeight)
    local targetX = math.floor(math.max(workX, math.min(maximumX, x)))
    local targetY = math.floor(math.max(workY, math.min(maximumY, y)))
    if targetX == math.floor(x) and targetY == math.floor(y) then return end

    SKIN:Bang('!SetWindowPosition', tostring(targetX), tostring(targetY))
    lastPositionX = targetX
    lastPositionY = targetY
    positionStableTicks = 0
            positionClampCooldown = 10
end

local function updatePositionClamp()
    if compactRestorePending and math.abs(currentWidgetHeight - targetWidgetHeight) < 0.7 then
        restoreCompactPosition()
        return
    end

    local x = tonumber(SKIN:GetVariable('CURRENTCONFIGX'))
    local y = tonumber(SKIN:GetVariable('CURRENTCONFIGY'))
    if not x or not y then return end

    if lastPositionX == nil or x ~= lastPositionX or y ~= lastPositionY then
        lastPositionX = x
        lastPositionY = y
        positionStableTicks = 0
    else
        positionStableTicks = positionStableTicks + 1
    end

    if positionClampCooldown > 0 then
        positionClampCooldown = positionClampCooldown - 1
        return
    end
    if positionClampDelay > 0 then
        positionClampDelay = positionClampDelay - 1
        if positionClampDelay == 0 then clampToWorkArea() end
        return
    end
    if positionStableTicks >= 16 then
        positionStableTicks = 0
        clampToWorkArea()
    end
end

local function launchBridge(force)
    if fileExists(bridgePath) and (force or bridgeLaunchCooldown <= 0) then
        SKIN:Bang('!CommandMeasure', 'MeasureBridgeLauncher', 'Run')
        bridgeLaunchCooldown = 50
    end
end

function Initialize()
    dataPath = SELF:GetOption('DataPath')
    queuePath = dataPath .. '\\Queue\\'
    bridgePath = SELF:GetOption('BridgePath')
    expectedBridgeVersion = SELF:GetOption('ExpectedBridgeVersion', '')
    SKIN:Bang('!AutoSelectScreen', '1')
    updateView(true)
    animationInitialized = true
    updateVisibility(true)
    schedulePositionClamp()
    if settingFlag('WelcomeCompleted', false) == 0 then
        SKIN:Bang('!DeactivateConfig', 'SonyXM5\\Settings')
        SKIN:Bang('!ActivateConfig', 'SonyXM5\\Welcome', 'Welcome.ini')
    end
    launchBridge(true)
end

function Update()
    if bridgeLaunchCooldown > 0 then bridgeLaunchCooldown = bridgeLaunchCooldown - 1 end
    local snapshot = readIni(dataPath .. '\\state.ini')
    -- Keep the last complete snapshot if Rainmeter happens to read while the
    -- bridge is publishing a replacement file. This prevents visible resets.
    if snapshot.bridge_version and snapshot.status then state = snapshot end
    if not fileExists(bridgePath) then
        state.status = 'setup'
        state.status_text = 'Bridge needs to be built'
        state.error = 'Run Scripts\\Build-Bridge.ps1, then refresh the skin.'
    elseif expectedBridgeVersion ~= '' and state.bridge_version
        and state.bridge_version ~= '' and state.bridge_version ~= expectedBridgeVersion then
        if not bridgeUpgradeActive then
            bridgeUpgradeActive = true
            bridgeUpgradeTimer = 6
            bridgeUpgradeCommand = Command('shutdown')
        else
            bridgeUpgradeTimer = bridgeUpgradeTimer - 1
        end

        if bridgeUpgradeTimer <= 0 or state.status == 'stopped' then
            if bridgeUpgradeCommand then os.remove(bridgeUpgradeCommand) end
            bridgeUpgradeCommand = nil
            launchBridge(true)
            bridgeUpgradeTimer = 6
        end

        state.status = 'updating'
        state.status_text = 'Updating headphone controls'
        state.error = 'Replacing the previous bridge helper.'
    else
        bridgeUpgradeActive = false
        bridgeUpgradeCommand = nil
        if state.status == 'starting' then
            startupStallTicks = startupStallTicks + 1
            if startupStallTicks == 100 then
                startupRecoveryCommand = Command('shutdown')
                startupRecoveryTimer = 10
            elseif startupStallTicks > 100 then
                startupRecoveryTimer = startupRecoveryTimer - 1
                if startupRecoveryTimer <= 0 then
                    if startupRecoveryCommand then os.remove(startupRecoveryCommand) end
                    startupRecoveryCommand = nil
                    launchBridge(true)
                    startupRecoveryTimer = 50
                end
            end
        else
            if startupRecoveryCommand then os.remove(startupRecoveryCommand) end
            startupRecoveryCommand = nil
            startupStallTicks = 0
            startupRecoveryTimer = 0
        end
        if state.status == 'stopped' then launchBridge(false) end
    end
    preserveStateDuringRecovery()
    preserveOptimisticState()
    preserveOptimisticVolume()
    updateBatteryAlert()
    if powerArmTicks > 0 then powerArmTicks = powerArmTicks - 1 end
    updateView(false)
    updateVisibility(false)
    applyState()
    updateAnimations()
    updatePositionClamp()
    return number(state.battery, 0)
end

function Command(command)
    commandCounter = commandCounter + 1
    local tick = math.floor((os.clock() % 1) * 100000000)
    local name = string.format('%010d-%08d-%04d.cmd', os.time(), tick, commandCounter)
    local path = queuePath .. name
    local file = io.open(path, 'w')
    if not file then
        SKIN:Bang('!Log', 'SonyXM5: could not write bridge command', 'Error')
        return nil
    end
    file:write(command, '\n')
    file:close()
    if command ~= 'shutdown' then commandPulseTicks = 12 end
    return path
end

function AdjustVolume(delta)
    delta = math.floor(number(delta, 0))
    if delta == 0 then return end

    local base = optimisticVolume
    if base == nil then base = number(state.volume, 0) end
    optimisticVolume = math.max(0, math.min(30, base + delta))
    optimisticVolumeTicks = 30
    state.volume = tostring(optimisticVolume)

    applyState()
    SKIN:Bang('!UpdateMeter', 'MeterVolumeFill')
    SKIN:Bang('!UpdateMeter', 'MeterVolumeValue')
    SKIN:Bang('!Redraw')
    Command('volume-step ' .. tostring(delta))
end

function MediaAction(action)
    action = tostring(action):lower()
    if action == 'playpause' then
        local playback = state.playback == 'playing' and 'paused' or 'playing'
        setOptimisticState('playback', playback)
        renderImmediate()
        Command('play-pause')
    elseif action == 'previous' or action == 'next' then
        Command(action)
    end
end

function SetAnc(mode)
    mode = tostring(mode):lower()
    local stateMode = mode
    if mode == 'noise' then stateMode = 'noise_cancelling' end
    if mode ~= 'noise' and mode ~= 'ambient' and mode ~= 'off' then return end
    setOptimisticState('anc_mode', stateMode)
    renderImmediate()
    Command('anc ' .. mode)
end

function SetAmbient(level)
    level = math.max(1, math.min(20, math.floor(number(level, 20))))
    setOptimisticState('ambient_level', level)
    renderImmediate()
    Command('ambient ' .. tostring(level))
end

function ToggleControl(key, command)
    local value = state[key] == '1' and '0' or '1'
    setOptimisticState(key, value)
    renderImmediate()
    Command(command)
end

function ToggleAdvanced()
    if not detailsVisible then
        detailsVisible = true
        advancedVisible = true
    else
        advancedVisible = not advancedVisible
    end
    updateView(true)
    updateVisibility(true)
    schedulePositionClamp()
    SKIN:Bang('!UpdateMeter', '*')
    SKIN:Bang('!Redraw')
end

function ToggleAdvancedControl(key, command, supportedKey)
    supportedKey = tostring(supportedKey or '')
    if supportedKey ~= '' and state[supportedKey] ~= '1' then return end
    ToggleControl(key, command)
end

function CycleEq(direction)
    if state.supported_eq ~= '1' then return end
    direction = tostring(direction):lower()
    local presets = {'Off', 'Bright', 'Excited', 'Mellow', 'Relaxed', 'Vocal', 'Treble boost', 'Bass boost', 'Speech', 'Custom', 'Custom 1', 'Custom 2'}
    local current = state.eq_preset or 'Off'
    local index = 1
    for position, value in ipairs(presets) do if value == current then index = position break end end
    if direction == 'previous' then
        index = ((index - 2) % #presets) + 1
    else
        direction = 'next'
        index = (index % #presets) + 1
    end
    setOptimisticState('eq_preset', presets[index])
    renderImmediate()
    Command('eq-' .. direction)
end

function AdjustEq(target, delta)
    if state.supported_eq ~= '1' then return end
    delta = math.floor(number(delta, 0))
    if delta == 0 then return end
    target = tostring(target):lower()
    local key
    local command
    if target == 'bass' then
        key = 'eq_bass'
        command = 'eq-bass-delta ' .. tostring(delta)
    else
        local band = math.max(1, math.min(5, math.floor(number(target, 1))))
        key = 'eq_band_' .. tostring(band)
        command = 'eq-band-delta ' .. tostring(band - 1) .. ' ' .. tostring(delta)
    end
    local value = math.max(-10, math.min(10, number(state[key], 0) + delta))
    setOptimisticState(key, value)
    renderImmediate()
    Command(command)
end

function CyclePriority()
    local value = state.priority == 'Sound quality' and 'Stable connection' or 'Sound quality'
    setOptimisticState('priority', value)
    renderImmediate()
    Command('priority')
end

function CycleAutoOff()
    local values = {'Off', '5 minutes', '15 minutes', '30 minutes', '60 minutes', '180 minutes'}
    local index = 1
    for position, value in ipairs(values) do if value == state.auto_off then index = position break end end
    index = (index % #values) + 1
    setOptimisticState('auto_off', values[index])
    renderImmediate()
    Command('auto-off-next')
end

function PowerOff()
    if state.supported_power_off ~= '1' then return end
    if powerArmTicks > 0 then
        powerArmTicks = 0
        Command('power-off')
    else
        powerArmTicks = 30
    end
    renderImmediate()
end

function RefreshBridge()
    Command('refresh')
end

function TestBatteryAlert()
    triggerBatteryAlert(settingNumber('BatteryAlertLevel', 20, 5, 50))
end

function Reconnect()
    Command('reconnect')
end

function CycleAnc()
    local mode = state.anc_mode or 'off'
    if mode == 'noise_cancelling' then
        SetAnc('ambient')
    elseif mode == 'ambient' then
        SetAnc('off')
    else
        SetAnc('noise')
    end
end

function ToggleDetails()
    SetDetails(not detailsVisible)
end

function SetDetails(value)
    local wasVisible = detailsVisible
    local nextVisible = value == true or tostring(value):lower() == 'true' or tostring(value) == '1'
    if nextVisible and not wasVisible then
        if compactRestorePending and compactReturnX ~= nil and compactReturnY ~= nil then
            compactRestorePending = false
        else
            captureCompactPosition()
        end
    end

    detailsVisible = nextVisible
    if not detailsVisible then advancedVisible = false end
    updateView(true)
    updateVisibility(true)
    if detailsVisible and not wasVisible then
        clampExpandedToWorkArea()
    elseif not detailsVisible and wasVisible and compactReturnX ~= nil and compactReturnY ~= nil then
        compactRestorePending = true
        if math.abs(currentWidgetHeight - targetWidgetHeight) < 0.7 then
            restoreCompactPosition()
        else
            schedulePositionClamp()
        end
    else
        schedulePositionClamp()
    end
    SKIN:Bang('!UpdateMeter', '*')
    SKIN:Bang('!Redraw')
end

function RefreshLayout()
    updateView(true)
    updateVisibility(true)
    applyState()
    schedulePositionClamp()
    SKIN:Bang('!UpdateMeter', '*')
    SKIN:Bang('!Redraw')
end

function DockBottom()
    local x = tonumber(SKIN:GetVariable('CURRENTCONFIGX')) or 0
    local workX = tonumber(SKIN:GetVariable('WORKAREAX')) or 0
    local workY = tonumber(SKIN:GetVariable('WORKAREAY')) or 0
    local workWidth = tonumber(SKIN:GetVariable('WORKAREAWIDTH')) or currentWidgetWidth
    local workHeight = tonumber(SKIN:GetVariable('WORKAREAHEIGHT')) or currentWidgetHeight
    local renderedWidth = tonumber(SKIN:GetVariable('CURRENTCONFIGWIDTH')) or currentWidgetWidth
    local renderedHeight = tonumber(SKIN:GetVariable('CURRENTCONFIGHEIGHT')) or currentWidgetHeight
    local targetX = math.floor(math.max(workX, math.min(workX + workWidth - renderedWidth, x)))
    local targetY = math.floor(workY + workHeight - renderedHeight)
    SKIN:Bang('!SetWindowPosition', tostring(targetX), tostring(targetY))
    lastPositionX = targetX
    lastPositionY = targetY
    positionStableTicks = 0
    positionClampCooldown = 16
end

function OpenSettings()
    SKIN:Bang('!ActivateConfig', 'SonyXM5\\Settings', 'Settings.ini')
end

function OpenFull()
    SKIN:Bang('!ActivateConfig', 'SonyXM5', 'Main.ini')
end
