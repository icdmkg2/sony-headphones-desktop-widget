; LINE advanced drawer -----------------------------------------------------

[StyleLineAdvancedText]
FontFace=#Font#
FontSize=#CurrentSecondarySize#
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=LeftCenter
H=28
DynamicVariables=1

[StyleLineAdvancedStep]
FontFace=#FontDisplay#
FontSize=(#CurrentPrimarySize#+1)
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=CenterCenter
W=20
H=28
DynamicVariables=1

[StyleLineAdvancedFeature]
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#UserTextColor#
AntiAlias=1
StringAlign=CenterCenter
W=#AdvancedHalfWidth#
H=34
ClipString=1
DynamicVariables=1

[MeterLineAdvancedDivider]
Meter=Shape
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#AdvancedTopY#
Shape=Line 0,0.5,#CurrentContentWidth#,0.5 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterLineAdvancedTitle]
Meter=String
Group=AdvancedLine
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

[MeterLineAdvancedInfo]
Meter=String
Group=AdvancedLine
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

[MeterLineEqLabel]
Meter=String
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=#AdvancedEqY#
W=82
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=LeftCenter
Text=EQ PRESET
DynamicVariables=1

[MeterLineEqPrevious]
Meter=String
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedEqPreviousX#
Y=#AdvancedEqY#
W=36
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=CenterCenter
Text=PREV
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleEq('previous')"]

[MeterLineEqPreset]
Meter=String
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedEqValueX#
Y=#AdvancedEqY#
W=112
H=28
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

[MeterLineEqNext]
Meter=String
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedEqNextX#
Y=#AdvancedEqY#
W=36
H=28
FontFace=#FontDisplay#
FontSize=#CurrentSecondarySize#
FontColor=#AdvancedEqColor#
AntiAlias=1
StringAlign=CenterCenter
Text=NEXT
DynamicVariables=1
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleEq('next')"]

; EQ row one: Clear Bass, band 1, band 2

[MeterLineEqBass]
Meter=String
MeterStyle=StyleLineAdvancedText
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell1X#
Y=#AdvancedBandRow1Y#
W=(#AdvancedCellWidth#-52)
Text=BASS  #EqBass#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq('bass',1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq('bass',-1)"]

[MeterLineEqBassMinus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell1MinusX#
Y=#AdvancedBandRow1Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq('bass',-1)"]

[MeterLineEqBassPlus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell1PlusX#
Y=#AdvancedBandRow1Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq('bass',1)"]

[MeterLineEqBand1]
Meter=String
MeterStyle=StyleLineAdvancedText
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell2X#
Y=#AdvancedBandRow1Y#
W=(#AdvancedCellWidth#-52)
Text=BAND 1  #EqBand1#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(1,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(1,-1)"]

[MeterLineEqBand1Minus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell2MinusX#
Y=#AdvancedBandRow1Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(1,-1)"]

[MeterLineEqBand1Plus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell2PlusX#
Y=#AdvancedBandRow1Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(1,1)"]

[MeterLineEqBand2]
Meter=String
MeterStyle=StyleLineAdvancedText
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell3X#
Y=#AdvancedBandRow1Y#
W=(#AdvancedCellWidth#-52)
Text=BAND 2  #EqBand2#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(2,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(2,-1)"]

[MeterLineEqBand2Minus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell3MinusX#
Y=#AdvancedBandRow1Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(2,-1)"]

[MeterLineEqBand2Plus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell3PlusX#
Y=#AdvancedBandRow1Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(2,1)"]

; EQ row two: bands 3-5

[MeterLineEqBand3]
Meter=String
MeterStyle=StyleLineAdvancedText
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell1X#
Y=#AdvancedBandRow2Y#
W=(#AdvancedCellWidth#-52)
Text=BAND 3  #EqBand3#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(3,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(3,-1)"]

[MeterLineEqBand3Minus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell1MinusX#
Y=#AdvancedBandRow2Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(3,-1)"]

[MeterLineEqBand3Plus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell1PlusX#
Y=#AdvancedBandRow2Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(3,1)"]

[MeterLineEqBand4]
Meter=String
MeterStyle=StyleLineAdvancedText
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell2X#
Y=#AdvancedBandRow2Y#
W=(#AdvancedCellWidth#-52)
Text=BAND 4  #EqBand4#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(4,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(4,-1)"]

[MeterLineEqBand4Minus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell2MinusX#
Y=#AdvancedBandRow2Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(4,-1)"]

[MeterLineEqBand4Plus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell2PlusX#
Y=#AdvancedBandRow2Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(4,1)"]

[MeterLineEqBand5]
Meter=String
MeterStyle=StyleLineAdvancedText
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell3X#
Y=#AdvancedBandRow2Y#
W=(#AdvancedCellWidth#-52)
Text=BAND 5  #EqBand5#
MouseScrollUpAction=[!CommandMeasure MeasureScript "AdjustEq(5,1)"]
MouseScrollDownAction=[!CommandMeasure MeasureScript "AdjustEq(5,-1)"]

[MeterLineEqBand5Minus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell3MinusX#
Y=#AdvancedBandRow2Y#
Text=-
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(5,-1)"]

[MeterLineEqBand5Plus]
Meter=String
MeterStyle=StyleLineAdvancedStep
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedCell3PlusX#
Y=#AdvancedBandRow2Y#
Text=+
LeftMouseUpAction=[!CommandMeasure MeasureScript "AdjustEq(5,1)"]

[MeterLineAdvancedFeatureDivider]
Meter=Shape
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#CurrentSidePadding#
Y=(#AdvancedFeatureRow1Y#-22)
Shape=Line 0,0.5,#CurrentContentWidth#,0.5 | Stroke Color #UserDividerColor# | StrokeWidth 1
DynamicVariables=1

[MeterLineMultipoint]
Meter=String
MeterStyle=StyleLineAdvancedFeature
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedHalf1Center#
Y=#AdvancedFeatureRow1Y#
FontColor=#AdvancedMultiColor#
Text=#AdvancedMultiLabel#
LeftMouseUpAction=[!CommandMeasure MeasureScript "ToggleAdvancedControl('multipoint','multipoint','supported_multipoint')"]

[MeterLinePriority]
Meter=String
MeterStyle=StyleLineAdvancedFeature
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedHalf2Center#
Y=#AdvancedFeatureRow1Y#
Text=#AdvancedPriorityLabel#
LeftMouseUpAction=[!CommandMeasure MeasureScript "CyclePriority()"]

[MeterLineAutoOff]
Meter=String
MeterStyle=StyleLineAdvancedFeature
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedHalf1Center#
Y=#AdvancedFeatureRow2Y#
Text=#AdvancedAutoOffLabel#
LeftMouseUpAction=[!CommandMeasure MeasureScript "CycleAutoOff()"]

[MeterLinePowerOff]
Meter=String
MeterStyle=StyleLineAdvancedFeature
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=#AdvancedHalf2Center#
Y=#AdvancedFeatureRow2Y#
FontColor=#AdvancedPowerColor#
Text=#AdvancedPowerLabel#
LeftMouseUpAction=[!CommandMeasure MeasureScript "PowerOff()"]

[MeterLineAdvancedBottom]
Meter=Shape
Group=AdvancedLine
Container=MeterBounds
Hidden=1
X=0
Y=#AdvancedBottomY#
Shape=Line 0,0.5,#WidgetWidth#,0.5 | Stroke Color 0,0,0,1 | StrokeWidth 1
DynamicVariables=1
