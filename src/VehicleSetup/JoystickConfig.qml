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

/// Joystick Config
SetupPage {
    id:                 joystickPage
    pageComponent:      pageComponent
    pageName:           qsTr("Joystick")
    pageDescription:    "" // qsTr("Joystick Setup is used to configure and calibrate joysticks.")

    readonly property real  _maxButtons:         64
    readonly property real  _attitudeLabelWidth: ScreenTools.defaultFontPixelWidth * 12

    // Override this, it is the only way of showing that warning when vehicle armed so Joystick settings can't be changed
    _disableDueToArmed: globals.activeVehicle ? globals.activeVehicle.armed : false
    
    // This property enables reusing this menu and all its childs for different joysticks
    property bool isSecondary: false

    Connections {
        target: joystickManager
        onAvailableJoysticksChanged: {
            if(joystickManager.joysticks.length === 0) {
                summaryButton.checked = true
                setupView.showSummaryPanel()
            }
        }
    }

    Component {
        id: pageComponent
        Item {
            width:  availableWidth
            height: bar.height + joyLoader.height

            readonly property real  labelToMonitorMargin:   ScreenTools.defaultFontPixelWidth * 3

            property var  _activeJoystick:          controller.isSecondary ? joystickManager.activeJoystickSecondary : joystickManager.activeJoystick
            property bool _allowJoystickSelection:  QGroundControl.corePlugin.options.allowJoystickSelection

            function setupPageCompleted() {
                controller.start()
            }

            JoystickConfigController {
                id: controller        
                
                property bool initialized: false
                
                // It is important to initialize JoystickConfigController like this so we can specify if it refers to primary or secondary joystick from qml
                Component.onCompleted: {
                    controller.isSecondary = joystickPage.isSecondary
                    controller.init()
                    initialized = true
                }
            }

            QGCTabBar {
                id:             bar
                width:          parent.width
                Component.onCompleted: {
                    if (_activeJoystick) {
                        if (_activeJoystick.axisCount == 0) {
                            currentIndex = _allowJoystickSelection ? 0 : 1
                        } else {
                            currentIndex = _activeJoystick.calibrated ? 0 : 2
                        }
                    } else {
                        currentIndex = 0
                    }
                }
                anchors.top:    parent.top
                QGCTabButton {
                    text:       qsTr("General")
                    visible:    _allowJoystickSelection
                }
                QGCTabButton {
                    text:       qsTr("Button Assigment")
                }
                QGCTabButton {
                    text:       qsTr("Calibration")
                    visible:    _activeJoystick.axisCount != 0
                }
                QGCTabButton {
                    text:       qsTr("Advanced")
                    visible:    _activeJoystick.axisCount != 0
                }
            }

            property var pages:  ["JoystickConfigGeneral.qml", "JoystickConfigButtons.qml", "JoystickConfigCalibration.qml", "JoystickConfigAdvanced.qml"]

            Loader {
                id:             joyLoader
                source:         controller.initialized ? pages[bar.currentIndex] : undefined
                width:          parent.width
                anchors.top:    bar.bottom
            }
        }
    }
}


