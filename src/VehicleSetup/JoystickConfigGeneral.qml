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
    width:  mainCol.width  + (ScreenTools.defaultFontPixelWidth  * 2)
    height: mainCol.height + (ScreenTools.defaultFontPixelHeight * 2)

    readonly property real axisMonitorWidth: ScreenTools.defaultFontPixelWidth * 32

    property bool _buttonsOnly:         _activeJoystick.axisCount == 0
    property bool _enableAllowed:       joystickManager.multiJoystickConfigOk
    property bool _axisUsed:         !_buttonsOnly && (_activeJoystick.mainControlEnabled || _activeJoystick.gimbalEnabled)

    Column {
        id:                 mainCol
        anchors.centerIn:   parent
        spacing:            ScreenTools.defaultFontPixelHeight
        GridLayout {
            columns:            2
            columnSpacing:      ScreenTools.defaultFontPixelWidth
            rowSpacing:         ScreenTools.defaultFontPixelHeight
            //---------------------------------------------------------------------
            //-- Enable Joystick
            QGCLabel {
                text:               qsTr("Enable joystick input for this vehicle ") + (joystickManager.multiJoystickConfigMessage ? qsTr("(Not allowed)") : "")
                Layout.alignment:   Qt.AlignVCenter
                Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 36
            }
            QGCCheckBox {
                id:             enabledSwitch
                enabled:        _enableAllowed
                onClicked:      {
                    globals.activeVehicle.joystickEnabled = checked
                    globals.activeVehicle.saveJoystickSettings()
                }
                Component.onCompleted: {
                    checked = globals.activeVehicle.joystickEnabled
                }
                Connections {
                    target: globals.activeVehicle
                    onJoystickEnabledChanged: {
                        enabledSwitch.checked = globals.activeVehicle.joystickEnabled
                    }
                }
                Connections {
                    target: joystickManager
                    onActiveJoystickChanged: {
                        if(_activeJoystick) {
                            enabledSwitch.checked = Qt.binding(function() { return _activeJoystick.calibrated && globals.activeVehicle.joystickEnabled })
                        }
                    }
                }
            }
            // Messages about what is wrong with multi joystick configuration in case joystick is not allowed because of it
            QGCLabel {
                id:                 multiJoystickConfigMessageLabel
                text:               joystickManager.multiJoystickConfigMessage
                Layout.fillWidth:   true
                Layout.columnSpan:  2
                font.bold:          true
                wrapMode:           Text.WordWrap
                visible:            !_enableAllowed
                color:              "red"
            }
            QGCLabel {
                text:               qsTr("When enabled joysticks will send control commands to the vehicle. ") +
                                    qsTr("If disabled, joysticks will not send any command at all to the vehicle")

                font.pointSize:     ScreenTools.smallFontPointSize
                Layout.fillWidth:   true
                Layout.columnSpan:  2
                wrapMode:           Text.WordWrap
                visible:            !multiJoystickConfigMessageLabel.visible
            }
            // Separator bar
            Rectangle {
                height: 2
                Layout.fillWidth:   true
                Layout.columnSpan:  2
                color:              qgcPal.windowShade
            }
            //---------------------------------------------------------------------
            //-- Joystick Selector
            QGCLabel {
                text:               joystickPrefix + qsTr("Joystick:")
                Layout.alignment:   Qt.AlignVCenter
                enabled:            globals.activeVehicle.joystickEnabled

                property var joystickPrefix: joystickManager.activeJoystickSecondary != undefined ? 
                                                " " + (controller.isSecondary ? qsTr("Secondary") : qsTr("Primary")) + " " : 
                                                " "
            }
            QGCComboBox {
                id:                 joystickCombo
                width:              ScreenTools.defaultFontPixelWidth * 40
                Layout.alignment:   Qt.AlignVCenter
                model:              joystickManager.joystickNames

                function showWarningSameJoysticks() {
                    mainWindow.showMessageDialog(qsTr("Wrong Joystick Setup"), qsTr("Primary and secondary Joysticks can not be the same"))
                }

                function updateCurrentJoystickIndex() {
                    var index = joystickCombo.find(controller.isSecondary ? joystickManager.activeJoystickSecondaryName : joystickManager.activeJoystickName)
                        if (index >= 0) {
                            joystickCombo.currentIndex = index
                        }
                }
                
                onActivated:        {
                    // Throw warning and return if user attempts to set both joysticks to the same one, although it would be handled in the backend as well anyway
                    if (controller.isSecondary) { 
                        if (textAt(index) === joystickManager.activeJoystickName) {
                            showWarningSameJoysticks()
                        } else {
                            joystickManager.activeJoystickSecondaryName = textAt(index)
                        }
                    } else {
                        if (textAt(index) === joystickManager.activeJoystickSecondaryName) {
                            showWarningSameJoysticks()
                        } else {
                            joystickManager.activeJoystickName = textAt(index)
                        }
                    }
                    // Update index, in case Joystick manager refuses the active joystick set, because of conflicts
                    // like attempting to set primary and secondary joystick to the same one, etc
                    updateCurrentJoystickIndex()
                }

                Component.onCompleted: {
                    var index = joystickCombo.find(controller.isSecondary ? joystickManager.activeJoystickSecondaryName : joystickManager.activeJoystickName)
                    if (index === -1) {
                        console.warn(qsTr("Active joystick name not in combo"), controller.isSecondary ? joystickManager.activeJoystickSecondaryName : joystickManager.activeJoystickName)
                    } else {
                        joystickCombo.currentIndex = index
                    }
                }
                Connections {
                    target: joystickManager
                    onAvailableJoysticksChanged: {
                        joystickCombo.updateCurrentJoystickIndex()
                    }
                    onActiveJoystickChanged: {
                        joystickCombo.updateCurrentJoystickIndex()
                    }
                    onActiveJoystickSecondaryChanged: {
                        joystickCombo.updateCurrentJoystickIndex()
                    }
                }
            }
            // --------------------------------------------------------------------
            // -- Other joystick indicator
            // Even if it is not this submenu's joystick, it is interesting to be aware that there is another active joystick from this menu as well
            QGCLabel {
                text:               buttonForgetSecondary.visible ? qsTr("Secondary Joystick: last used not connected, but a new one is present") :
                                        controller.isSecondary ? 
                                            qsTr("Primary Joystick also active: ") :
                                                qsTr("Secondary Joystick also active: ")

                visible:            otherActiveJoystick != undefined || (joystickManager.joysticks.length > 1)
                Layout.fillWidth:   true
                enabled:            globals.activeVehicle.joystickEnabled

                property var otherActiveJoystick: controller.isSecondary ? joystickManager.activeJoystick : joystickManager.activeJoystickSecondary
                property var otherActiveJoystickName : otherActiveJoystick ? otherActiveJoystick.name : ""
            }
            QGCLabel {
                text:               otherActiveJoystickName

                visible:            otherActiveJoystick != undefined || (joystickManager.joysticks.length > 1)
                Layout.fillWidth:   true
                enabled:            globals.activeVehicle.joystickEnabled

                property var otherActiveJoystick: controller.isSecondary ? joystickManager.activeJoystick : joystickManager.activeJoystickSecondary
                property var otherActiveJoystickName : otherActiveJoystick ? otherActiveJoystick.name : ""
            }
            // --------------------------------------------------------------------
            // -- Forget existent second joystick button: 
            // If we have 2 joysticks working, but we disconnect one of them and want to use a third one, we can not seem to make the second menu appear
            // automatically as out of safety we prevent this ( to manage gracefully reconections of joysticks being used ). We need to be able to "reset" this behaviour
            // to set up a new secondary joystick
            QGCButton {
                // we can reuse instead the combobox as well, just making sure it is only visible in primary, and refers always to secondary
                id:                 buttonForgetSecondary
                text:               qsTr("Forget latest second joystick and set up a new one")
                visible:            !controller.isSecondary && joystickManager.joysticks.length > 1 && !joystickManager.activeJoystickSecondary
                Layout.fillWidth:   true
                Layout.columnSpan:  2
                onClicked:          joystickManager.resetJoystickPrimarySecondarySettings()
            }
            // Separator bar
            Rectangle {
                height: 2
                Layout.fillWidth:   true
                Layout.columnSpan:  2
                color:              qgcPal.windowShade
            }
            //---------------------------------------------------------------------
            //-- RC Mode
            QGCLabel {
                text:               qsTr("RC Mode:")
                Layout.alignment:   Qt.AlignVCenter
                visible:            _activeJoystick.mainControlEnabled
                enabled:            globals.activeVehicle.joystickEnabled
            }
            Row {
                spacing:            ScreenTools.defaultFontPixelWidth
                visible:            _activeJoystick.mainControlEnabled
                QGCRadioButton {
                    text:       "1"
                    checked:    controller.transmitterMode === 1
                    enabled:    !controller.calibrating
                    onClicked:  controller.transmitterMode = 1
                    anchors.verticalCenter: parent.verticalCenter
                }
                QGCRadioButton {
                    text:       "2"
                    checked:    controller.transmitterMode === 2
                    enabled:    !controller.calibrating
                    onClicked:  controller.transmitterMode = 2
                    anchors.verticalCenter: parent.verticalCenter
                }
                QGCRadioButton {
                    text:       "3"
                    checked:    controller.transmitterMode === 3
                    enabled:    !controller.calibrating
                    onClicked:  controller.transmitterMode = 3
                    anchors.verticalCenter: parent.verticalCenter
                }
                QGCRadioButton {
                    text:       "4"
                    checked:    controller.transmitterMode === 4
                    enabled:    !controller.calibrating
                    onClicked:  controller.transmitterMode = 4
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
        Row {
            spacing:                ScreenTools.defaultFontPixelWidth
            enabled:                globals.activeVehicle.joystickEnabled
            //---------------------------------------------------------------------
            //-- Axis Monitors
            Rectangle {
                id:                 axisRect
                color:              Qt.rgba(0,0,0,0)
                border.color:       qgcPal.text
                border.width:       1
                radius:             ScreenTools.defaultFontPixelWidth * 0.5
                width:              axisGrid.width  + (ScreenTools.defaultFontPixelWidth  * 2)
                height:             axisGrid.height + (ScreenTools.defaultFontPixelHeight * 2)
                visible:            _axisUsed
                GridLayout {
                    id:                 axisGrid
                    columns:            2
                    columnSpacing:      ScreenTools.defaultFontPixelWidth
                    rowSpacing:         ScreenTools.defaultFontPixelHeight
                    anchors.centerIn:   parent
                    QGCLabel {
                        text:               globals.activeVehicle.sub ? qsTr("Lateral") : qsTr("Roll")
                        Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 12
                        visible:            _activeJoystick.mainControlEnabled
                    }
                    AxisMonitor {
                        id:                 rollAxis
                        height:             ScreenTools.defaultFontPixelHeight
                        width:              axisMonitorWidth
                        mapped:             controller.rollAxisMapped
                        reversed:           controller.rollAxisReversed
                        visible:            _activeJoystick.mainControlEnabled
                    }

                    QGCLabel {
                        id:                 pitchLabel
                        width:              _attitudeLabelWidth
                        text:               globals.activeVehicle.sub ? qsTr("Forward") : qsTr("Pitch")
                        visible:            _activeJoystick.mainControlEnabled
                    }
                    AxisMonitor {
                        id:                 pitchAxis
                        height:             ScreenTools.defaultFontPixelHeight
                        width:              axisMonitorWidth
                        mapped:             controller.pitchAxisMapped
                        reversed:           controller.pitchAxisReversed
                        visible:            _activeJoystick.mainControlEnabled
                    }

                    QGCLabel {
                        id:                 yawLabel
                        width:              _attitudeLabelWidth
                        text:               qsTr("Yaw")
                        visible:            _activeJoystick.mainControlEnabled
                    }
                    AxisMonitor {
                        id:                 yawAxis
                        height:             ScreenTools.defaultFontPixelHeight
                        width:              axisMonitorWidth
                        mapped:             controller.yawAxisMapped
                        reversed:           controller.yawAxisReversed
                        visible:            _activeJoystick.mainControlEnabled
                    }

                    QGCLabel {
                        id:                 throttleLabel
                        width:              _attitudeLabelWidth
                        text:               qsTr("Throttle")
                        visible:            _activeJoystick.mainControlEnabled
                    }
                    AxisMonitor {
                        id:                 throttleAxis
                        height:             ScreenTools.defaultFontPixelHeight
                        width:              axisMonitorWidth
                        mapped:             controller.throttleAxisMapped
                        reversed:           controller.throttleAxisReversed
                        visible:            _activeJoystick.mainControlEnabled
                    }

                    QGCLabel {
                        id:                 gimbalPitchLabel
                        width:              _attitudeLabelWidth
                        text:               qsTr("Gimbal Pitch")
                        visible:            _activeJoystick && _activeJoystick.gimbalEnabled
                    }
                    AxisMonitor {
                        id:                 gimbalPitchAxis
                        height:             ScreenTools.defaultFontPixelHeight
                        width:              axisMonitorWidth
                        mapped:             controller.gimbalPitchAxisMapped
                        reversed:           controller.gimbalPitchAxisReversed
                        visible:            _activeJoystick && _activeJoystick.gimbalEnabled
                    }

                    QGCLabel {
                        id:                 gimbalYawLabel
                        width:              _attitudeLabelWidth
                        text:               qsTr("Gimbal Yaw")
                        visible:            _activeJoystick && _activeJoystick.gimbalEnabled
                    }
                    AxisMonitor {
                        id:                 gimbalYawAxis
                        height:             ScreenTools.defaultFontPixelHeight
                        width:              axisMonitorWidth
                        mapped:             controller.gimbalYawAxisMapped
                        reversed:           controller.gimbalYawAxisReversed
                        visible:            _activeJoystick && _activeJoystick.gimbalEnabled
                    }

                    Connections {
                        target:             _activeJoystick
                        onAxisValues: {
                            rollAxis.axisValue          = roll  * 32768.0
                            pitchAxis.axisValue         = pitch * 32768.0
                            yawAxis.axisValue           = yaw   * 32768.0
                            throttleAxis.axisValue      = _activeJoystick.negativeThrust ? throttle * -32768.0 : (-2 * throttle + 1) * 32768.0
                            gimbalPitchAxis.axisValue   = gimbalPitch * 32768.0
                            gimbalYawAxis.axisValue     = gimbalYaw   * 32768.0
                        }
                    }
                }
            }
            Rectangle {
                color:              Qt.rgba(0,0,0,0)
                border.color:       qgcPal.text
                border.width:       1
                radius:             ScreenTools.defaultFontPixelWidth * 0.5
                width:              axisRect.visible ? axisRect.width :  buttonsFlow.width * 1.2
                height:             axisRect.visible ? axisRect.height : buttonsFlow.height * 1.2  
                Flow {
                    id:                 buttonsFlow
                    width:              ScreenTools.defaultFontPixelWidth * 30
                    spacing:            -1
                    anchors.centerIn:   parent
                    Connections {
                        target:     _activeJoystick
                        onRawButtonPressedChanged: {
                            if (buttonMonitorRepeater.itemAt(index)) {
                                buttonMonitorRepeater.itemAt(index).pressed = pressed
                            }
                        }
                    }
                    Repeater {
                        id:         buttonMonitorRepeater
                        model:      _activeJoystick ? _activeJoystick.totalButtonCount : []
                        Rectangle {
                            width:          ScreenTools.defaultFontPixelHeight * 1.5
                            height:         width
                            border.width:   1
                            border.color:   qgcPal.text
                            color:          pressed ? qgcPal.buttonHighlight : qgcPal.windowShade
                            property bool pressed
                            QGCLabel {
                                anchors.fill:           parent
                                color:                  pressed ? qgcPal.buttonHighlightText : qgcPal.buttonText
                                horizontalAlignment:    Text.AlignHCenter
                                verticalAlignment:      Text.AlignVCenter
                                text:                   modelData
                            }
                        }
                    }
                }
            }
        }
    }
}


