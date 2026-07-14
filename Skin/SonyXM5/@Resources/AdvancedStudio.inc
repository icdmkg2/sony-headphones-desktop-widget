; STUDIO advanced drawer ---------------------------------------------------

[StyleStudioAdvancedText]
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=LeftCenter
H=30
DynamicVariables=1

[StyleStudioAdvancedStep]
FontFace=#FontDisplay#
FontSize=(#CurrentPrimarySize#+1)
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=CenterCenter
W=20
H=30
DynamicVariables=1

[StyleStudioAdvancedFeature]
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=CenterCenter
W=#AdvancedHalfWidth#
H=34
ClipString=1
DynamicVariables=1

[MeterStudioAdvancedDivider]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#AdvancedTopY#
Shape=Line 0,0.5,#CurrentContentWidth#,0.5 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterStudioAdvancedTitle]
Meter=String
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#AdvancedHeaderY#
W=110
H=24
FontFace=#FontDisplay#
FontSize=(#CurrentSecondarySize#+1)
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=LeftCenter
Text=ADVANCED
DynamicVariables=1

[MeterStudioAdvancedInfo]
Meter=String
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=(#CurrentSidePadding#+#CurrentContentWidth#)
Y=#AdvancedHeaderY#
W=(#CurrentContentWidth#-112)
H=22
FontFace=#Font#
FontSize=#CurrentSecondarySize#
FontColor=#UserMutedColor#
AntiAlias=1
StringAlign=RightCenter
ClipString=1
Text=#AdvancedInfo#
DynamicVariables=1

[MeterStudioEqLabel]
Meter=String
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#AdvancedEqY#
W=82
H=30
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=LeftCenter
Text=EQ PRESET
DynamicVariables=1

[MeterStudioEqBox]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedEqBoxX#
Y=(#AdvancedEqY#-17)
Shape=Rectangle 0.5,0.5,203,33,11 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
Shape2=Line 44.5,6,44.5,28 | Stroke Color #UserDividerColor# | StrokeWidth 1
Shape3=Line 160.5,6,160.5,28 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterStudioEqPrevious]
Meter=String
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedEqPreviousX#
Y=#AdvancedEqY#
W=44
H=34
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=CenterCenter
Text=PREV
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleEq('previous')"]

[MeterStudioEqPreset]
Meter=String
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedEqValueX#
Y=#AdvancedEqY#
W=116
H=34
FontFace=#FontDisplay#
FontSize=(#CurrentSecondarySize#+1)
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=CenterCenter
ClipString=1
Text=#EqPreset#
DynamicVariables=1
MouseScrollUpAction=[!CommandMeasure MeasureScript "CycleEq('next')"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "CycleEq('previous')"]

[MeterStudioEqNext]
Meter=String
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedEqNextX#
Y=#AdvancedEqY#
W=44
H=34
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=CenterCenter
Text=NEXT
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleEq('next')"]

; EQ row one: Clear Bass, band 1, band 2

[MeterStudioEqBassBox]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell1X#
Y=(#AdvancedBandRow1Y#-15)
Shape=Rectangle 0.5,0.5,(#AdvancedCellWidth#-1),29,10 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq('bass',1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq('bass',-1)"]

[MeterStudioEqBass]
Meter=String
MeterStyle=StyleStudioAdvancedText
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=(#AdvancedCell1X#+10)
Y=#AdvancedBandRow1Y#
W=(#AdvancedCellWidth#-54)
Text=BASS  #EqBass#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq('bass',1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq('bass',-1)"]

[MeterStudioEqBassMinus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell1MinusX#
Y=#AdvancedBandRow1Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq('bass',-1)"]

[MeterStudioEqBassPlus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell1PlusX#
Y=#AdvancedBandRow1Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq('bass',1)"]

[MeterStudioEqBand1Box]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell2X#
Y=(#AdvancedBandRow1Y#-15)
Shape=Rectangle 0.5,0.5,(#AdvancedCellWidth#-1),29,10 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(1,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(1,-1)"]

[MeterStudioEqBand1]
Meter=String
MeterStyle=StyleStudioAdvancedText
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=(#AdvancedCell2X#+10)
Y=#AdvancedBandRow1Y#
W=(#AdvancedCellWidth#-54)
Text=BAND 1  #EqBand1#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(1,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(1,-1)"]

[MeterStudioEqBand1Minus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell2MinusX#
Y=#AdvancedBandRow1Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(1,-1)"]

[MeterStudioEqBand1Plus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell2PlusX#
Y=#AdvancedBandRow1Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(1,1)"]

[MeterStudioEqBand2Box]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell3X#
Y=(#AdvancedBandRow1Y#-15)
Shape=Rectangle 0.5,0.5,(#AdvancedCellWidth#-1),29,10 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(2,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(2,-1)"]

[MeterStudioEqBand2]
Meter=String
MeterStyle=StyleStudioAdvancedText
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=(#AdvancedCell3X#+10)
Y=#AdvancedBandRow1Y#
W=(#AdvancedCellWidth#-54)
Text=BAND 2  #EqBand2#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(2,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(2,-1)"]

[MeterStudioEqBand2Minus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell3MinusX#
Y=#AdvancedBandRow1Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(2,-1)"]

[MeterStudioEqBand2Plus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell3PlusX#
Y=#AdvancedBandRow1Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(2,1)"]

; EQ row two: bands 3-5

[MeterStudioEqBand3Box]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell1X#
Y=(#AdvancedBandRow2Y#-15)
Shape=Rectangle 0.5,0.5,(#AdvancedCellWidth#-1),29,10 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(3,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(3,-1)"]

[MeterStudioEqBand3]
Meter=String
MeterStyle=StyleStudioAdvancedText
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=(#AdvancedCell1X#+10)
Y=#AdvancedBandRow2Y#
W=(#AdvancedCellWidth#-54)
Text=BAND 3  #EqBand3#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(3,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(3,-1)"]

[MeterStudioEqBand3Minus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell1MinusX#
Y=#AdvancedBandRow2Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(3,-1)"]

[MeterStudioEqBand3Plus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell1PlusX#
Y=#AdvancedBandRow2Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(3,1)"]

[MeterStudioEqBand4Box]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell2X#
Y=(#AdvancedBandRow2Y#-15)
Shape=Rectangle 0.5,0.5,(#AdvancedCellWidth#-1),29,10 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(4,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(4,-1)"]

[MeterStudioEqBand4]
Meter=String
MeterStyle=StyleStudioAdvancedText
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=(#AdvancedCell2X#+10)
Y=#AdvancedBandRow2Y#
W=(#AdvancedCellWidth#-54)
Text=BAND 4  #EqBand4#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(4,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(4,-1)"]

[MeterStudioEqBand4Minus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell2MinusX#
Y=#AdvancedBandRow2Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(4,-1)"]

[MeterStudioEqBand4Plus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell2PlusX#
Y=#AdvancedBandRow2Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(4,1)"]

[MeterStudioEqBand5Box]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell3X#
Y=(#AdvancedBandRow2Y#-15)
Shape=Rectangle 0.5,0.5,(#AdvancedCellWidth#-1),29,10 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(5,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(5,-1)"]

[MeterStudioEqBand5]
Meter=String
MeterStyle=StyleStudioAdvancedText
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=(#AdvancedCell3X#+10)
Y=#AdvancedBandRow2Y#
W=(#AdvancedCellWidth#-54)
Text=BAND 5  #EqBand5#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(5,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(5,-1)"]

[MeterStudioEqBand5Minus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell3MinusX#
Y=#AdvancedBandRow2Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(5,-1)"]

[MeterStudioEqBand5Plus]
Meter=String
MeterStyle=StyleStudioAdvancedStep
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedCell3PlusX#
Y=#AdvancedBandRow2Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(5,1)"]

[MeterStudioMultipointBox]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedHalf1X#
Y=(#AdvancedFeatureRow1Y#-17)
Shape=Rectangle 0.5,0.5,(#AdvancedHalfWidth#-1),33,11 | Fill Color #ChipMultiFill# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleAdvancedControl('multipoint','multipoint','supported_multipoint')"]

[MeterStudioMultipoint]
Meter=String
MeterStyle=StyleStudioAdvancedFeature
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedHalf1Center#
Y=#AdvancedFeatureRow1Y#
FontColor=#ChipMultiText#
Text=#AdvancedMultiLabel#
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleAdvancedControl('multipoint','multipoint','supported_multipoint')"]

[MeterStudioPriorityBox]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedHalf2X#
Y=(#AdvancedFeatureRow1Y#-17)
Shape=Rectangle 0.5,0.5,(#AdvancedHalfWidth#-1),33,11 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CyclePriority()"]

[MeterStudioPriority]
Meter=String
MeterStyle=StyleStudioAdvancedFeature
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedHalf2Center#
Y=#AdvancedFeatureRow1Y#
Text=#AdvancedPriorityLabel#
LeftMouseUpAction=[!CommandMeasure MeasureScript "CyclePriority()"]

[MeterStudioAutoOffBox]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedHalf1X#
Y=(#AdvancedFeatureRow2Y#-17)
Shape=Rectangle 0.5,0.5,(#AdvancedHalfWidth#-1),33,11 | Fill Color #UserControlFillColor# | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleAutoOff()"]

[MeterStudioAutoOff]
Meter=String
MeterStyle=StyleStudioAdvancedFeature
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedHalf1Center#
Y=#AdvancedFeatureRow2Y#
Text=#AdvancedAutoOffLabel#
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleAutoOff()"]

[MeterStudioPowerOffBox]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedHalf2X#
Y=(#AdvancedFeatureRow2Y#-17)
Shape=Rectangle 0.5,0.5,(#AdvancedHalfWidth#-1),33,11 | Fill Color #UserControlFillColor# | Stroke Color #AdvancedPowerColor# | StrokeWidth 1
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "PowerOff()"]

[MeterStudioPowerOff]
Meter=String
MeterStyle=StyleStudioAdvancedFeature
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=#AdvancedHalf2Center#
Y=#AdvancedFeatureRow2Y#
FontColor=#AdvancedPowerColor#
Text=#AdvancedPowerLabel#
LeftMouseUpAction=[!CommandMeasure MeasureScript "PowerOff()"]

[MeterStudioAdvancedBottom]
Meter=Shape
Group=AdvancedStudio
Container=MeterBounds
Hidden=1
X=0
Y=#AdvancedBottomY#
Shape=Line 0,0.5,#WidgetWidth#,0.5 | Stroke Color 0,0,0,1 | StrokeWidth 1
DynamicVariables=1
