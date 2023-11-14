/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick                      2.11
import QtQuick.Controls             2.4
import QtQuick.Dialogs              1.3
import QtQuick.Layouts              1.11

import QGroundControl               1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controllers   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0

Item {
    width:                  grid.width  + (ScreenTools.defaultFontPixelWidth  * 2)
    height:                 grid.height + (ScreenTools.defaultFontPixelHeight * 2)

    property var            parentJoystick: _activeJoystick

    // We should never need this as SetupView should manage it
    onParentJoystickChanged: {
        if(_activeJoystick) {
            enabledMainControl.checked = Qt.binding(function() { return _activeJoystick.mainControlEnabled })
            enabledGimbal.checked = Qt.binding(function() { return _activeJoystick.gimbalEnabled })
        }
    }
    //---------------------------------------------------------------------
    GridLayout {
        id:                 grid
        columns:            2
        columnSpacing:      ScreenTools.defaultFontPixelWidth
        rowSpacing:         ScreenTools.defaultFontPixelHeight
        anchors.centerIn:   parent
        //-------------------------------------------------------------
        //-------------------------------------------------------------
        QGCRadioButton {
            text:               qsTr("Full down stick is zero throttle")
            checked:            _activeJoystick ? _activeJoystick.throttleMode === 1 : false
            onClicked:          _activeJoystick.throttleMode = 1
            Layout.columnSpan:  2
        }
        QGCRadioButton {
            text:               qsTr("Center stick is zero throttle")
            checked:            _activeJoystick ? _activeJoystick.throttleMode === 0 : false
            onClicked:          _activeJoystick.throttleMode = 0
            Layout.columnSpan:  2
        }
        //-------------------------------------------------------------
        QGCLabel {
            text:               qsTr("Spring loaded throttle smoothing")
            visible:            _activeJoystick ? _activeJoystick.throttleMode === 0 : false
            Layout.alignment:   Qt.AlignVCenter
            Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 36
        }
        QGCCheckBox {
            checked:            _activeJoystick ? _activeJoystick.accumulator : false
            visible:            _activeJoystick ? _activeJoystick.throttleMode === 0 : false
            onClicked:          _activeJoystick.accumulator = checked
        }
        //-------------------------------------------------------------
        QGCLabel {
            text:               qsTr("Allow negative Thrust")
            visible:            globals.activeVehicle.supportsNegativeThrust
            Layout.alignment:   Qt.AlignVCenter
        }
        QGCCheckBox {
            visible:            globals.activeVehicle.supportsNegativeThrust
            enabled:            globals.activeVehicle.supportsNegativeThrust
            checked:            _activeJoystick ? _activeJoystick.negativeThrust : false
            onClicked:          _activeJoystick.negativeThrust = checked
        }
        //---------------------------------------------------------------------
        QGCLabel {
            text:               qsTr("Exponential:")
        }
        Row {
            spacing:            ScreenTools.defaultFontPixelWidth
            QGCSlider {
                id:             expoSlider
                width:          ScreenTools.defaultFontPixelWidth * 20
                minimumValue:   0
                maximumValue:   0.75
                Component.onCompleted: value = -_activeJoystick.exponential
                onValueChanged: _activeJoystick.exponential = -value
             }
            QGCLabel {
                id:     expoSliderIndicator
                text:   expoSlider.value.toFixed(2)
            }
        }
        //---------------------------------------------------------------------
        //-- Disable Main Control
        QGCLabel {
            text:               qsTr("Enable Main Control")
            Layout.alignment:   Qt.AlignVCenter
        }
        QGCCheckBox {
            id:                 enabledMainControl
            enabled:            _activeJoystick
            onClicked:          _activeJoystick.mainControlEnabled = checked
            Component.onCompleted: {
                checked = _activeJoystick.mainControlEnabled
            }
        }
        QGCLabel {
            text:               qsTr("Unclicking this option will disable main controls ( throttle, roll, ") +
                                qsTr("pitch and yaw ) in this joystick. This is desired if the joystick ") +
                                qsTr("is only needed for gimbal control, or only for using buttons")

            font.pointSize:     ScreenTools.smallFontPointSize
            Layout.alignment:   Qt.AlignVCenter
            Layout.columnSpan:  2
            wrapMode:           Text.WordWrap
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 30 
        }
        //---------------------------------------------------------------------
        //-- Enable Gimbal
        QGCLabel {
            text:               qsTr("Enable gimbal control")
            Layout.alignment:   Qt.AlignVCenter
        }
        QGCCheckBox {
            id:                 enabledGimbal
            enabled:            _activeJoystick
            onClicked:          _activeJoystick.gimbalEnabled = checked
            Component.onCompleted: {
                checked = _activeJoystick.gimbalEnabled
            }
        }
        QGCLabel {
            text:               qsTr("Gimbal control in Ardupilot will only work after parameters ") +
                                qsTr("are downloaded. It will check RCX_FUCNTION until finding gimbal ") +
                                qsTr("pitch/yaw (213/214), and will send an RC_OVERRIDE mavlink message ") +
                                qsTr("just for those channels. Roll, pitch, yaw and throttle will work ") +
                                qsTr("before parameters are Downloaded")
                                
            font.pointSize:     ScreenTools.smallFontPointSize
            Layout.fillWidth:   true
            Layout.columnSpan:  2
            wrapMode:           Text.WordWrap
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 30 
        }
        //-----------------------------------------------------------------
        //-- Axis Message Frequency
        QGCLabel {
            text:               qsTr("Axis frequency (Hz):")
            Layout.alignment:   Qt.AlignVCenter
        }
        QGCTextField {
            text:               _activeJoystick.axisFrequencyHz
            validator:          DoubleValidator { bottom: _activeJoystick.minAxisFrequencyHz; top: _activeJoystick.maxAxisFrequencyHz; }
            inputMethodHints:   Qt.ImhFormattedNumbersOnly
            Layout.alignment:   Qt.AlignVCenter
            onEditingFinished: {
                _activeJoystick.axisFrequencyHz = parseFloat(text)
            }
        }
        //-----------------------------------------------------------------
        //-- Button Repeat Frequency
        QGCLabel {
            text:               qsTr("Button repeat frequency (Hz):")
            Layout.alignment:   Qt.AlignVCenter
        }
        QGCTextField {
            text:               _activeJoystick.buttonFrequencyHz
            validator:          DoubleValidator { bottom: _activeJoystick.minButtonFrequencyHz; top: _activeJoystick.maxButtonFrequencyHz; }
            inputMethodHints:   Qt.ImhFormattedNumbersOnly
            Layout.alignment:   Qt.AlignVCenter
            onEditingFinished: {
                _activeJoystick.buttonFrequencyHz = parseFloat(text)
            }
        }
        //-----------------------------------------------------------------
        //-- Enable circle correction
        QGCLabel {
            text:               qsTr("Enable circle correction")
            Layout.alignment:   Qt.AlignVCenter
        }
        QGCCheckBox {
            checked:            globals.activeVehicle.joystickMode !== 0
            Component.onCompleted: {
                checked = _activeJoystick.circleCorrection
            }
            onClicked: {
                _activeJoystick.circleCorrection = checked
            }
        }
        //-----------------------------------------------------------------
        //-- Deadband
        QGCLabel {
            text:               qsTr("Deadbands")
            Layout.alignment:   Qt.AlignVCenter
        }
        QGCCheckBox {
            checked:            controller.deadbandToggle
            onClicked:          controller.deadbandToggle = checked
            Layout.alignment:   Qt.AlignVCenter
        }
        QGCLabel{
            Layout.fillWidth:   true
            Layout.columnSpan:  2
            font.pointSize:     ScreenTools.smallFontPointSize
            wrapMode:           Text.WordWrap
            text:   qsTr("Deadband can be set during the first ") +
                    qsTr("step of calibration by gently wiggling each axis. ") +
                    qsTr("Deadband can also be adjusted by clicking and ") +
                    qsTr("dragging vertically on the corresponding axis monitor.")
        }
    }
}


