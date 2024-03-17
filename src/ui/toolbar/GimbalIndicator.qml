/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick         2.11
import QtQuick.Layouts 1.11

import QGroundControl                     1.0
import QGroundControl.Controls            1.0
import QGroundControl.MultiVehicleManager 1.0
import QGroundControl.ScreenTools         1.0
import QGroundControl.Palette             1.0

Item {
    id:             _root
    width:          gimbalIndicatorIcon.width * 1.1 + gimbalTelemetryLayout.childrenRect.width + margins
    anchors.top:    parent.top
    anchors.bottom: parent.bottom

    property var    activeVehicle:      QGroundControl.multiVehicleManager.activeVehicle
    property var    gimbalController:   activeVehicle.gimbalController
    property bool   showIndicator:      gimbalController && gimbalController.gimbals.count
    property var    activeGimbal:       gimbalController.activeGimbal
    property var    multiGimbalSetup:   gimbalController.gimbals.count > 1

    property var    margins:            ScreenTools.defaultFontPixelWidth
    property var    panelRadius:        ScreenTools.defaultFontPixelWidth * 0.5
    property var    buttonHeight:       height * 1.6

    // Popup panel
    Component {
        id: remoteIDInfo

        Rectangle {
            width:          mainLayout.width   + mainLayout.anchors.margins * 2          
            height:         mainLayout.height  + mainLayout.anchors.margins * 2
            color:          qgcPal.window
            // border.color:   qgcPal.text
            radius:         panelRadius

            GridLayout {
                id:                 mainLayout
                anchors.margins:    ScreenTools.defaultFontPixelWidth
                anchors.top:        parent.top
                anchors.right:      parent.right

                QGCLabel {
                    text:                   qsTr("Gimbal ") + 
                                                (multiGimbalSetup ? activeGimbal.deviceId.rawValue : "") + 
                                                    qsTr("<br> Controls")

                    font.pointSize:         ScreenTools.smallFontPointSize
                    Layout.preferredWidth:  buttonHeight * 1.1
                    font.weight:            Font.Medium
                }

                QGCButton {
                    Layout.preferredWidth:  Layout.preferredHeight
                    Layout.preferredHeight: buttonHeight
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignVCenter
                    text:                   activeGimbal.yawLock ? qsTr("Yaw <br> Follow") : qsTr("Yaw <br> Lock")
                    fontWeight:             Font.Medium
                    pointSize:              ScreenTools.smallFontPointSize
                    backRadius:             panelRadius * 0.5
                    onClicked: {
                        gimbalController.toggleGimbalYawLock(!activeGimbal.yawLock)
                    }
                }

                QGCButton {
                    Layout.preferredWidth:  Layout.preferredHeight
                    Layout.preferredHeight: buttonHeight
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignVCenter
                    text:                   qsTr("Center")
                    fontWeight:             Font.Medium
                    pointSize:              ScreenTools.smallFontPointSize
                    backRadius:             panelRadius * 0.5
                    onClicked: {
                        gimbalController.centerGimbal()
                    }
                }

                QGCButton {
                    Layout.preferredWidth:  Layout.preferredHeight
                    Layout.preferredHeight: buttonHeight
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignVCenter
                    text:                   qsTr("Tilt 90")
                    fontWeight:             Font.Medium
                    pointSize:              ScreenTools.smallFontPointSize
                    backRadius:             panelRadius * 0.5
                    onClicked: {
                        gimbalController.sendPitchBodyYaw(-90, 0)
                    }
                }

                QGCButton {
                    Layout.preferredWidth:  Layout.preferredHeight
                    Layout.preferredHeight: buttonHeight
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignVCenter
                    text:                   qsTr("Point < br> Home")
                    fontWeight:             Font.Medium
                    pointSize:              ScreenTools.smallFontPointSize
                    backRadius:             panelRadius * 0.5
                    onClicked: {
                        activeVehicle.guidedModeROI(activeVehicle.homePosition)
                    }
                }

                QGCButton {
                    Layout.preferredWidth:  Layout.preferredHeight
                    Layout.preferredHeight: buttonHeight
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignVCenter
                    text:                   qsTr("Retract")
                    fontWeight:             Font.Medium
                    pointSize:              ScreenTools.smallFontPointSize
                    backRadius:             panelRadius * 0.5
                    onClicked: {
                        gimbalController.toggleGimbalRetracted(true)
                    }
                }

                QGCButton {
                    id:                     gimbalSelectorButton
                    Layout.leftMargin:      margins
                    Layout.preferredWidth:  Layout.preferredHeight
                    Layout.preferredHeight: buttonHeight
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignBottom
                    text:                   qsTr("Select <br> Active <br> Gimbal")
                    fontWeight:             Font.Medium
                    pointSize:              ScreenTools.smallFontPointSize
                    backRadius:             panelRadius * 0.5
                    visible:                gimbalController.gimbals.count > 1
                    checkable:              true

                    // This rectangle is to hide the "roundness" of panels when showing the dropdown
                    Rectangle {
                        id:                       hideRoundCornersRectangle
                        anchors.verticalCenter:   gimbalSelectorPanel.top
                        anchors.horizontalCenter: gimbalSelectorPanel.horizontalCenter
                        width:                    gimbalSelectorPanel.width
                        height:                   panelRadius * 2
                        color:                    qgcPal.window
                        visible:                  gimbalSelectorPanel.visible
                    }
                    
                    Rectangle {
                        id:        gimbalSelectorPanel
                        width:     buttonHeight + margins * 2
                        height:    gimbalSelectorContentGrid.childrenRect.height + margins * 2
                        visible:   gimbalSelectorButton.checked
                        color:     qgcPal.window
                        radius:    panelRadius

                        anchors.top:                parent.bottom
                        anchors.horizontalCenter:   parent.horizontalCenter
                        anchors.topMargin:          margins

                        property var buttonWidth:    width - margins * 2
                        property var panelHeight:    gimbalSelectorContentGrid.childrenRect.height + margins * 2
                        property var gridRowSpacing: margins
                        property var buttonFontSize: ScreenTools.smallFontPointSize * 0.9

                        GridLayout {
                            id:               gimbalSelectorContentGrid
                            width:            parent.width
                            rowSpacing:       gimbalSelectorPanel.gridRowSpacing
                            columns:          1

                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top:              parent.top
                            anchors.topMargin:        margins

                            Repeater {
                                model: gimbalController && gimbalController.gimbals ? gimbalController.gimbals : undefined
                                delegate: QGCButton {
                                    Layout.preferredWidth:  Layout.preferredHeight
                                    Layout.preferredHeight: buttonHeight
                                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignVCenter
                                    fontWeight:             Font.Medium
                                    pointSize:              ScreenTools.smallFontPointSize
                                    backRadius:             panelRadius * 0.5
                                    text:                   qsTr("Gimbal ") + object.deviceId.rawValue
                                    checked:                activeGimbal === object
                                    onClicked: {
                                        gimbalController.activeGimbal = object
                                        gimbalSelectorButton.checked = false
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }
    }

    // Icon plus active instance indicator
    Image {
        id:                      gimbalIndicatorIcon
        width:                   height
        anchors.top:             parent.top
        anchors.bottom:          parent.bottom
        source:                  "/gimbal/payload.svg"
        fillMode:                Image.PreserveAspectFit
        sourceSize.height:       height

        // Current active gimbal indicator
        QGCLabel {
            id:                  gimbalInstanceIndicatorLabel
            text:                activeGimbal ? activeGimbal.deviceId.rawValue : ""
            visible:             activeVehicle.gimbalController.gimbals.count > 1
            anchors.top:         parent.top
            anchors.topMargin:   -margins * 0.5
            anchors.right:       parent.right
            anchors.rightMargin: -margins * 0.5
        }
    }

    // Telemetry and status indicator
    GridLayout {
        id:                        gimbalTelemetryLayout
        anchors.left:              gimbalIndicatorIcon.right
        anchors.leftMargin:        margins
        anchors.verticalCenter:    parent.verticalCenter
        columns:                   2
        rows:                      2
        rowSpacing:                0
        columnSpacing:             margins

        QGCLabel {
            id:                         statusLabel
            text:                       activeGimbal && activeGimbal.retracted ? 
                                            qsTr("Retracted") :
                                            (activeGimbal && activeGimbal.yawLock ? qsTr("Yaw locked") : qsTr("Yaw follow"))
            Layout.columnSpan:          2
            Layout.alignment:           Qt.AlignHCenter
        }
        QGCLabel {
            id:                 pitchLabel
            text:               activeGimbal ? qsTr("P: ") + activeGimbal.absolutePitch.rawValue.toFixed(1) : ""
        }
        QGCLabel {
            id:                 panLabel
            text:               activeGimbal ? qsTr("Y: ") + activeGimbal.bodyYaw.rawValue.toFixed(1) : ""
        }
    }

    MouseArea {
        anchors.fill:   parent
        onClicked: {
            mainWindow.showIndicatorPopup(_root, remoteIDInfo, false)
        }
    }
}
