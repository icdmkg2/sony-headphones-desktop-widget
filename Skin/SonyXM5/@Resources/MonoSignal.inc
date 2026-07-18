; Mono Signal design
; All meters start hidden. SonyXM5.lua owns design and option visibility.

; Header -------------------------------------------------------------------

[MeterSignalBattery]
Meter=String
Group=SignalHeader
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#HeaderCenterY#
W=54
H=30
FontFace=#FontDisplay#
FontSize=(#CurrentPrimarySize#+5)
FontColor=#BatteryColor#
AntiAlias=1
StringAlign=LeftCenter
Text=#BatteryCompact#
DynamicVariables=1

[MeterSignalBatteryRail]
Meter=Shape
Group=SignalHeader
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=(#HeaderCenterY#+16)
Shape=Line 0,0,52,0 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterSignalBatteryFill]
Meter=Shape
Group=SignalHeader
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=(#HeaderCenterY#+16)
Shape=Line 0,0,#SignalBatteryFill#,0 | Stroke Color #BatteryColor# | StrokeWidth 2
DynamicVariables=1

[MeterSignalBatteryIcon]
Meter=Shape
Group=SignalHeader|SignalBatteryIcon
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+54)
Y=(#HeaderCenterY#-4)
Shape=Rectangle 0.5,0.5,13,7,1 | Fill Color 0,0,0,0 | Stroke Color #BatteryColor# | StrokeWidth 1
Shape2=Rectangle 2,2,#BatteryIconFillWidth#,4,0 | Fill Color #BatteryColor# | StrokeWidth 0
Shape3=Rectangle 15,2,2,4,0 | Fill Color #UserMutedColor# | StrokeWidth 0
DynamicVariables=1

[MeterSignalStatusBars]
Meter=Shape
Group=SignalHeader
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+67+#BatteryLayoutExtra#)
Y=(#HeaderCenterY#+#DeviceCenterOffset#-6)
Shape=Line 0,0,0,12 | Stroke Color #StatusColor# | StrokeWidth 2
Shape2=Line 5,3,5,12 | Stroke Color #StatusColor# | StrokeWidth 2
Shape3=Line 10,6,10,12 | Stroke Color #StatusColor# | StrokeWidth 2
DynamicVariables=1

[MeterSignalStatusPulse]
Meter=Shape
Group=SignalHeader
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+72+#BatteryLayoutExtra#)
Y=(#HeaderCenterY#+#DeviceCenterOffset#)
Shape=Ellipse 0,0,#StatusPulseRadius#,#StatusPulseRadius# | Fill Color 0,0,0,0 | Stroke Color #StatusPulseColor# | StrokeWidth 1
DynamicVariables=1

[MeterSignalDevice]
Meter=String
Group=SignalHeader
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+86+#BatteryLayoutExtra#)
Y=(#HeaderCenterY#+#DeviceCenterOffset#)
W=#SignalDeviceWidth#
H=16
FontFace=#FontDisplay#
FontSize=#CurrentPrimarySize#
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=LeftCenter
ClipString=1
Text=#DeviceName#
DynamicVariables=1

[MeterSignalStatus]
Meter=String
Group=SignalHeader
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+86+#BatteryLayoutExtra#)
Y=(#HeaderCenterY#+10)
W=#SignalDeviceWidth#
H=14
FontFace=#Font#
FontSize=#CurrentSecondarySize#
FontColor=#StatusColor#
AntiAlias=1
StringAlign=LeftCenter
ClipString=1
Text=#StatusText#
DynamicVariables=1
MouseActionCursor=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "StatusAction()"]

; Compact actions ----------------------------------------------------------

[MeterSignalCompactAncRule]
Meter=Shape
Group=SignalHeader|SignalCompactActions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-96)
Y=(#HeaderCenterY#+16)
Shape=Line 0,0,56,0 | Stroke Color #MonoAncCurrent# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleAnc()"]

[MeterSignalCompactAnc]
Meter=String
Group=SignalHeader|SignalCompactActions|SignalCompactButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-68)
Y=#HeaderCenterY#
W=56
H=30
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#MonoAncCurrent#
AntiAlias=1
StringAlign=CenterCenter
Text=#MiniAncLabel#
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleAnc()"]

[MeterSignalCompactAncNoiseIcon]
Meter=Shape
Group=SignalHeader|SignalCompactActions|SignalCompactButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-96)
Y=(#HeaderCenterY#-15)
Shape=Rectangle 0,0,56,30,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 28,15,8,8 | Fill Color 0,0,0,0 | Stroke Color #CompactAncNoiseIconColor# | StrokeWidth 1.5
Shape3=Ellipse 28,15,3,3 | Fill Color 0,0,0,0 | Stroke Color #CompactAncNoiseIconColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleAnc()"]

[MeterSignalCompactAncAmbientIcon]
Meter=Shape
Group=SignalHeader|SignalCompactActions|SignalCompactButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-96)
Y=(#HeaderCenterY#-15)
Shape=Rectangle 0,0,56,30,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Line 21,13,21,17 | Stroke Color #CompactAncAmbientIconColor# | StrokeWidth 1.5
Shape3=Line 26,9,26,21 | Stroke Color #CompactAncAmbientIconColor# | StrokeWidth 1.5
Shape4=Line 31,11,31,19 | Stroke Color #CompactAncAmbientIconColor# | StrokeWidth 1.5
Shape5=Line 36,14,36,16 | Stroke Color #CompactAncAmbientIconColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleAnc()"]

[MeterSignalCompactAncOffIcon]
Meter=Shape
Group=SignalHeader|SignalCompactActions|SignalCompactButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-96)
Y=(#HeaderCenterY#-15)
Shape=Rectangle 0,0,56,30,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 28,15,8,8 | Fill Color 0,0,0,0 | Stroke Color #CompactAncOffIconColor# | StrokeWidth 1.5
Shape3=Line 22,21,34,9 | Stroke Color #CompactAncOffIconColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleAnc()"]

[MeterSignalExpand]
Meter=String
Group=SignalHeader|SignalCompactActions|SignalCompactButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-16)
Y=#HeaderCenterY#
W=32
H=30
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=CenterCenter
Text=MORE
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleDetails()"]

[MeterSignalExpandIcon]
Meter=Shape
Group=SignalHeader|SignalCompactActions|SignalCompactButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-32)
Y=(#HeaderCenterY#-15)
Shape=Rectangle 0,0,32,30,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Line 10,12,16,18 | Stroke Color #UserMutedColor# | StrokeWidth 2
Shape3=Line 16,18,22,12 | Stroke Color #UserMutedColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleDetails()"]

; Expanded header actions --------------------------------------------------

[MeterSignalSettings]
Meter=String
Group=SignalHeader|SignalExpandedAction|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-70)
Y=#HeaderCenterY#
W=38
H=30
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=CenterCenter
Text=SET
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "OpenSettings()"]

[MeterSignalSettingsIcon]
Meter=Shape
Group=SignalHeader|SignalExpandedAction|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-89)
Y=(#HeaderCenterY#-15)
Shape=Rectangle 0,0,38,30,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 19,15,6,6 | Fill Color 0,0,0,0 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape3=Ellipse 19,15,2,2 | Fill Color 0,0,0,0 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape4=Line 19,5,19,8 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape5=Line 19,22,19,25 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape6=Line 9,15,12,15 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape7=Line 26,15,29,15 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape8=Line 12,8,14,10 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape9=Line 24,20,26,22 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape10=Line 12,22,14,20 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape11=Line 24,10,26,8 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "OpenSettings()"]

[MeterSignalCollapse]
Meter=String
Group=SignalHeader|SignalExpandedAction|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-20)
Y=#HeaderCenterY#
W=40
H=30
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=CenterCenter
Text=LESS
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleDetails()"]

[MeterSignalCollapseIcon]
Meter=Shape
Group=SignalHeader|SignalExpandedAction|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-40)
Y=(#HeaderCenterY#-15)
Shape=Rectangle 0,0,40,30,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Line 14,18,20,12 | Stroke Color #UserMutedColor# | StrokeWidth 2
Shape3=Line 20,12,26,18 | Stroke Color #UserMutedColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleDetails()"]

; Expanded mode selector ---------------------------------------------------

[MeterSignalDetailTop]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#DetailTopY#
Shape=Line 0,0.5,#CurrentContentWidth#,0.5 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterSignalListeningMark]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=(#DetailTopY#+10)
Shape=Line 0,0,0,18 | Stroke Color #MonoAncCurrent# | StrokeWidth 3
DynamicVariables=1

[MeterSignalListeningLabel]
Meter=String
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+12)
Y=(#DetailTopY#+19)
W=82
H=20
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=LeftCenter
Text=LISTENING
DynamicVariables=1

[MeterSignalNoiseRule]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-194)
Y=(#DetailTopY#+32)
Shape=Line 0,0,52,0 | Stroke Color #MonoNoiseColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('noise')"]

[MeterSignalAmbientRule]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-130)
Y=(#DetailTopY#+32)
Shape=Line 0,0,76,0 | Stroke Color #MonoAmbientColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('ambient')"]

[MeterSignalOffRule]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-42)
Y=(#DetailTopY#+32)
Shape=Line 0,0,42,0 | Stroke Color #MonoOffColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('off')"]

[MeterSignalNoise]
Meter=String
Group=SignalDetails|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-168)
Y=(#DetailTopY#+18)
W=52
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#MonoNoiseColor#
AntiAlias=1
StringAlign=CenterCenter
Text=NC
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('noise')"]

[MeterSignalAmbient]
Meter=String
Group=SignalDetails|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-92)
Y=(#DetailTopY#+18)
W=76
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#MonoAmbientColor#
AntiAlias=1
StringAlign=CenterCenter
Text=AMBIENT
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('ambient')"]

[MeterSignalOff]
Meter=String
Group=SignalDetails|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-21)
Y=(#DetailTopY#+18)
W=42
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#MonoOffColor#
AntiAlias=1
StringAlign=CenterCenter
Text=OFF
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('off')"]

[MeterSignalNoiseIcon]
Meter=Shape
Group=SignalDetails|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-194)
Y=(#DetailTopY#+4)
Shape=Rectangle 0,0,52,28,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 26,14,7,7 | Fill Color 0,0,0,0 | Stroke Color #MonoNoiseColor# | StrokeWidth 1.5
Shape3=Ellipse 26,14,2.5,2.5 | Fill Color 0,0,0,0 | Stroke Color #MonoNoiseColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('noise')"]

[MeterSignalAmbientIcon]
Meter=Shape
Group=SignalDetails|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-130)
Y=(#DetailTopY#+4)
Shape=Rectangle 0,0,76,28,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Line 29,12,29,16 | Stroke Color #MonoAmbientColor# | StrokeWidth 1.5
Shape3=Line 35,8,35,20 | Stroke Color #MonoAmbientColor# | StrokeWidth 1.5
Shape4=Line 41,10,41,18 | Stroke Color #MonoAmbientColor# | StrokeWidth 1.5
Shape5=Line 47,13,47,15 | Stroke Color #MonoAmbientColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('ambient')"]

[MeterSignalOffIcon]
Meter=Shape
Group=SignalDetails|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-42)
Y=(#DetailTopY#+4)
Shape=Rectangle 0,0,42,28,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 21,14,7,7 | Fill Color 0,0,0,0 | Stroke Color #MonoOffColor# | StrokeWidth 1.5
Shape3=Line 16,19,26,9 | Stroke Color #MonoOffColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAnc('off')"]

; Ambient strength ---------------------------------------------------------

[MeterSignalAmbientLabel]
Meter=String
Group=SignalDetails|SignalAmbientOptions
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=(#DetailTopY#+58)
W=72
H=20
FontFace=#Font#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=LeftCenter
Text=AMBIENT  #AmbientLevel#
DynamicVariables=1

[MeterSignalAmbientRail]
Meter=Shape
Group=SignalDetails|SignalAmbientOptions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-210)
Y=(#DetailTopY#+58)
Shape=Line 0,0,190,0 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterSignalAmbient1]
Meter=Shape
Group=SignalDetails|SignalAmbientOptions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-210)
Y=(#DetailTopY#+58)
Shape=Rectangle -14,-12,28,24,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 0,0,3.5,3.5 | Fill Color #AmbMono1# | StrokeWidth 0
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAmbient(1)"]

[MeterSignalAmbient5]
Meter=Shape
Group=SignalDetails|SignalAmbientOptions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-162.5)
Y=(#DetailTopY#+58)
Shape=Rectangle -14,-12,28,24,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 0,0,3.5,3.5 | Fill Color #AmbMono5# | StrokeWidth 0
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAmbient(5)"]

[MeterSignalAmbient10]
Meter=Shape
Group=SignalDetails|SignalAmbientOptions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-115)
Y=(#DetailTopY#+58)
Shape=Rectangle -14,-12,28,24,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 0,0,3.5,3.5 | Fill Color #AmbMono10# | StrokeWidth 0
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAmbient(10)"]

[MeterSignalAmbient15]
Meter=Shape
Group=SignalDetails|SignalAmbientOptions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-67.5)
Y=(#DetailTopY#+58)
Shape=Rectangle -14,-12,28,24,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 0,0,3.5,3.5 | Fill Color #AmbMono15# | StrokeWidth 0
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAmbient(15)"]

[MeterSignalAmbient20]
Meter=Shape
Group=SignalDetails|SignalAmbientOptions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-20)
Y=(#DetailTopY#+58)
Shape=Rectangle -14,-12,28,24,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse 0,0,3.5,3.5 | Fill Color #AmbMono20# | StrokeWidth 0
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SetAmbient(20)"]

; Now playing and transport -------------------------------------------------

[MeterSignalPlaybackLine]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#PlaybackLineY#
Shape=Line 0,0.5,#CurrentContentWidth#,0.5 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterSignalNowPlaying]
Meter=String
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=(#PlaybackLineY#+10)
W=100
H=16
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=LeftCenter
Text=NOW PLAYING
DynamicVariables=1

[MeterSignalTrack]
Meter=String
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#TrackTitleY#
W=(#CurrentContentWidth#-198)
H=22
FontFace=#FontDisplay#
FontSize=(#CurrentPrimarySize#+1)
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=LeftCenter
ClipString=1
Text=#TrackTitle#
DynamicVariables=1

[MeterSignalArtist]
Meter=String
Group=SignalDetails|SignalArtistOption
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#TrackArtistY#
W=(#CurrentContentWidth#-198)
H=18
FontFace=#Font#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=LeftCenter
ClipString=1
Text=#TrackArtist#
DynamicVariables=1

[MeterSignalPrevious]
Meter=String
Group=SignalDetails|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-154)
Y=#PlaybackCenterY#
W=60
H=36
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=CenterCenter
Text=PREV
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "MediaAction('previous')"]

[MeterSignalPlayPause]
Meter=String
Group=SignalDetails|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-92)
Y=#PlaybackCenterY#
W=64
H=36
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=CenterCenter
Text=#PlaybackLabel#
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "MediaAction('playpause')"]

[MeterSignalNext]
Meter=String
Group=SignalDetails|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-30)
Y=#PlaybackCenterY#
W=60
H=36
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=CenterCenter
Text=NEXT
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "MediaAction('next')"]

[MeterSignalPreviousIcon]
Meter=Shape
Group=SignalDetails|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-184)
Y=(#PlaybackCenterY#-18)
Shape=Rectangle 0,0,60,36,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Line 34,11,26,18 | Stroke Color #UserMutedColor# | StrokeWidth 2
Shape3=Line 26,18,34,25 | Stroke Color #UserMutedColor# | StrokeWidth 2
Shape4=Line 23,11,23,25 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "MediaAction('previous')"]

[MeterSignalPlayPauseIcon]
Meter=String
Group=SignalDetails|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-92)
Y=#PlaybackCenterY#
W=64
H=36
FontFace=#FontDisplay#
FontSize=(#CurrentPrimarySize#+2)
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=CenterCenter
Text=#PlaybackIcon#
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "MediaAction('playpause')"]

[MeterSignalNextIcon]
Meter=Shape
Group=SignalDetails|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-60)
Y=(#PlaybackCenterY#-18)
Shape=Rectangle 0,0,60,36,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Line 26,11,34,18 | Stroke Color #UserMutedColor# | StrokeWidth 2
Shape3=Line 34,18,26,25 | Stroke Color #UserMutedColor# | StrokeWidth 2
Shape4=Line 37,11,37,25 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "MediaAction('next')"]

; Volume -------------------------------------------------------------------

[MeterSignalVolumeLabel]
Meter=String
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#VolumeCenterY#
W=78
H=26
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=LeftCenter
Text=VOL  #VolumePercent#%
DynamicVariables=1
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustVolume(1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustVolume(-1)"]

[MeterSignalVolumeRail]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+82)
Y=#VolumeCenterY#
Shape=Line 0,0,#SignalVolumeRailWidth#,0 | Stroke Color #UserDividerColor# | StrokeWidth 2
Shape2=Rectangle 0,-12,#SignalVolumeRailWidth#,24 | Fill Color 0,0,0,1 | StrokeWidth 0
DynamicVariables=1
MouseActionCursor=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SeekVolume($MouseX$ / #SignalVolumeRailWidth#)"]
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustVolume(1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustVolume(-1)"]

[MeterSignalVolumeFill]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+82)
Y=#VolumeCenterY#
Shape=Line 0,0,#SignalVolumeFill#,0 | Stroke Color #UserTextColor# | StrokeWidth 3
Shape2=Rectangle 0,-12,#SignalVolumeRailWidth#,24 | Fill Color 0,0,0,1 | StrokeWidth 0
DynamicVariables=1
MouseActionCursor=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "SeekVolume($MouseX$ / #SignalVolumeRailWidth#)"]
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustVolume(1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustVolume(-1)"]

[MeterSignalVolumeMinus]
Meter=String
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-64)
Y=#VolumeCenterY#
W=34
H=30
FontFace=#FontDisplay#
FontSize=(#CurrentPrimarySize#+2)
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=CenterCenter
Text=-
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustVolume(-1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustVolume(-1)"]

[MeterSignalVolumePlus]
Meter=String
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#-20)
Y=#VolumeCenterY#
W=34
H=30
FontFace=#FontDisplay#
FontSize=(#CurrentPrimarySize#+2)
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=CenterCenter
Text=+
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustVolume(1)"]
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustVolume(1)"]

; Quick actions ------------------------------------------------------------

[MeterSignalQuickLine]
Meter=Shape
Group=SignalDetails|SignalQuickOptions
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#QuickLineY#
Shape=Line 0,0.5,#CurrentContentWidth#,0.5 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterSignalSpeakRule]
Meter=Shape
Group=SignalDetails|SignalQuickOptions
Container=MeterBounds
Hidden=1
X=#StudioQuick1X#
Y=(#QuickCenterY#+14)
Shape=Line 0,0,#StudioQuickWidth#,0 | Stroke Color #MonoSpeakColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('speak_to_chat','speak-to-chat')"]

[MeterSignalDseeRule]
Meter=Shape
Group=SignalDetails|SignalQuickOptions
Container=MeterBounds
Hidden=1
X=#StudioQuick2X#
Y=(#QuickCenterY#+14)
Shape=Line 0,0,#StudioQuickWidth#,0 | Stroke Color #MonoDseeColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('dsee','dsee')"]

[MeterSignalWearRule]
Meter=Shape
Group=SignalDetails|SignalQuickOptions
Container=MeterBounds
Hidden=1
X=#StudioQuick3X#
Y=(#QuickCenterY#+14)
Shape=Line 0,0,#StudioQuickWidth#,0 | Stroke Color #MonoPauseColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('auto_pause','auto-pause')"]

[MeterSignalAdvancedRule]
Meter=Shape
Group=SignalDetails|SignalQuickOptions
Container=MeterBounds
Hidden=1
X=#StudioQuick4X#
Y=(#QuickCenterY#+14)
Shape=Line 0,0,#StudioQuickWidth#,0 | Stroke Color #UserMutedColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleAdvanced()"]

[MeterSignalSpeak]
Meter=String
Group=SignalDetails|SignalQuickOptions|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=#StudioQuick1Center#
Y=#QuickCenterY#
W=#StudioQuickWidth#
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#MonoSpeakColor#
AntiAlias=1
StringAlign=CenterCenter
Text=CHAT
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('speak_to_chat','speak-to-chat')"]

[MeterSignalDsee]
Meter=String
Group=SignalDetails|SignalQuickOptions|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=#StudioQuick2Center#
Y=#QuickCenterY#
W=#StudioQuickWidth#
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#MonoDseeColor#
AntiAlias=1
StringAlign=CenterCenter
Text=DSEE
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('dsee','dsee')"]

[MeterSignalWear]
Meter=String
Group=SignalDetails|SignalQuickOptions|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=#StudioQuick3Center#
Y=#QuickCenterY#
W=#StudioQuickWidth#
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#MonoPauseColor#
AntiAlias=1
StringAlign=CenterCenter
Text=WEAR
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('auto_pause','auto-pause')"]

[MeterSignalAdvanced]
Meter=String
Group=SignalDetails|SignalQuickOptions|SignalDetailButtonText
Container=MeterBounds
Hidden=1
X=#StudioQuick4Center#
Y=#QuickCenterY#
W=#StudioQuickWidth#
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=CenterCenter
Text=#AdvancedLabel#
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleAdvanced()"]

[MeterSignalSpeakIcon]
Meter=Shape
Group=SignalDetails|SignalQuickOptions|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=#StudioQuick1X#
Y=(#QuickCenterY#-14)
Shape=Rectangle 0,0,#StudioQuickWidth#,28,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Rectangle (#StudioQuickWidth#/2-8),7,16,11,2 | Fill Color 0,0,0,0 | Stroke Color #MonoSpeakColor# | StrokeWidth 1.5
Shape3=Line (#StudioQuickWidth#/2-3),18,(#StudioQuickWidth#/2-7),22 | Stroke Color #MonoSpeakColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('speak_to_chat','speak-to-chat')"]

[MeterSignalDseeIcon]
Meter=Shape
Group=SignalDetails|SignalQuickOptions|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=#StudioQuick2X#
Y=(#QuickCenterY#-14)
Shape=Rectangle 0,0,#StudioQuickWidth#,28,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Line (#StudioQuickWidth#/2-9),17,(#StudioQuickWidth#/2-9),11 | Stroke Color #MonoDseeColor# | StrokeWidth 2
Shape3=Line (#StudioQuickWidth#/2-3),20,(#StudioQuickWidth#/2-3),8 | Stroke Color #MonoDseeColor# | StrokeWidth 2
Shape4=Line (#StudioQuickWidth#/2+3),18,(#StudioQuickWidth#/2+3),10 | Stroke Color #MonoDseeColor# | StrokeWidth 2
Shape5=Line (#StudioQuickWidth#/2+9),16,(#StudioQuickWidth#/2+9),12 | Stroke Color #MonoDseeColor# | StrokeWidth 2
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('dsee','dsee')"]

[MeterSignalWearIcon]
Meter=Shape
Group=SignalDetails|SignalQuickOptions|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=#StudioQuick3X#
Y=(#QuickCenterY#-14)
Shape=Rectangle 0,0,#StudioQuickWidth#,28,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Ellipse (#StudioQuickWidth#/2),14,8,8 | Fill Color 0,0,0,0 | Stroke Color #MonoPauseColor# | StrokeWidth 1.5
Shape3=Line (#StudioQuickWidth#/2-3),10,(#StudioQuickWidth#/2-3),18 | Stroke Color #MonoPauseColor# | StrokeWidth 1.5
Shape4=Line (#StudioQuickWidth#/2+3),10,(#StudioQuickWidth#/2+3),18 | Stroke Color #MonoPauseColor# | StrokeWidth 1.5
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleControl('auto_pause','auto-pause')"]

[MeterSignalAdvancedIcon]
Meter=Shape
Group=SignalDetails|SignalQuickOptions|SignalDetailButtonIcons
Container=MeterBounds
Hidden=1
X=#StudioQuick4X#
Y=(#QuickCenterY#-14)
Shape=Rectangle 0,0,#StudioQuickWidth#,28,0 | Fill Color 0,0,0,1 | StrokeWidth 0
Shape2=Line (#StudioQuickWidth#/2-10),9,(#StudioQuickWidth#/2+10),9 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape3=Line (#StudioQuickWidth#/2-10),14,(#StudioQuickWidth#/2+10),14 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape4=Line (#StudioQuickWidth#/2-10),19,(#StudioQuickWidth#/2+10),19 | Stroke Color #UserMutedColor# | StrokeWidth 1.5
Shape5=Ellipse (#StudioQuickWidth#/2-4),9,2,2 | Fill Color #UserMutedColor# | StrokeWidth 0
Shape6=Ellipse (#StudioQuickWidth#/2+5),14,2,2 | Fill Color #UserMutedColor# | StrokeWidth 0
Shape7=Ellipse (#StudioQuickWidth#/2-2),19,2,2 | Fill Color #UserMutedColor# | StrokeWidth 0
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleAdvanced()"]

; Connection health --------------------------------------------------------

[MeterSignalHealthLine]
Meter=Shape
Group=SignalDetails|SignalHealthOptions
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#HealthLineY#
Shape=Line 0,0.5,#CurrentContentWidth#,0.5 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterSignalHealthBars]
Meter=Shape
Group=SignalDetails|SignalHealthOptions
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#HealthCenterY#
Shape=Line 0,0,5,0 | Stroke Color #HealthColor# | StrokeWidth 2
Shape2=Line 8,0,13,0 | Stroke Color #HealthColor# | StrokeWidth 2
Shape3=Line 16,0,21,0 | Stroke Color #HealthColor# | StrokeWidth 2
Shape4=Line 24,0,29,0 | Stroke Color #HealthColor# | StrokeWidth 2
DynamicVariables=1

[MeterSignalHealthLabel]
Meter=String
Group=SignalDetails|SignalHealthOptions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+42)
Y=#HealthCenterY#
W=92
H=24
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#HealthColor#
AntiAlias=1
StringAlign=LeftCenter
Text=#HealthLabel#
DynamicVariables=1

[MeterSignalHealthSummary]
Meter=String
Group=SignalDetails|SignalHealthOptions
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#)
Y=#HealthCenterY#
W=(#CurrentContentWidth#-144)
H=24
FontFace=#Font#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=RightCenter
ClipString=1
Text=#HealthSummary#
DynamicVariables=1

[MeterSignalBottom]
Meter=Shape
Group=SignalDetails
Container=MeterBounds
Hidden=1
X=0
Y=#DetailBottomY#
Shape=Line 0,0.5,#WidgetWidth#,0.5 | Stroke Color 0,0,0,1 | StrokeWidth 1
DynamicVariables=1
