import QtQuick 2.12

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0

Item {
    id:           rootItem
    anchors.fill: parent

    property var screenX
    property var screenY

    property var screenXrateInitCoocked
    property var screenYrateInitCoocked

    property var  _gimbalController:         _activeVehicle ? _activeVehicle.gimbalController : undefined
    property var  _activeGimbal:             _gimbalController ? _gimbalController.activeGimbal : undefined
    property bool _gimbalAvailable:          _activeGimbal != undefined
    property var  _gimbalControllerSettings: QGroundControl.settingsManager.gimbalControllerSettings
    property var  _activeVehicle:            QGroundControl.multiVehicleManager.activeVehicle

    property bool shouldProcessClicks:       _gimbalControllerSettings.EnableOnScreenControl.value && _activeGimbal

    // Functions for on screen gimbal control
    function clickControl() {
        if (!shouldProcessClicks) {
            return
        }
        if (!_gimbalControllerSettings.ControlType.rawValue == 0) {
            return
        }
        clickAndPoint(x, y)
    }

    // Sends a +-(0-1) xy value to vehicle.gimbalController.gimbalOnScreenControl
    function clickAndPoint() {
        if (rootItem._gimbalAvailable) {
            var xCoocked =  ( (screenX / parent.width)  * 2) - 1
            var yCoocked = -( (screenY / parent.height) * 2) + 1
            // console.log("X global: " + x + " Y global: " + y)
            // console.log("X coocked: " + xCoocked + " Y coocked: " + yCoocked)
            _gimbalController.gimbalOnScreenControl(xCoocked, yCoocked, true, false, false)
        } else {
            console.log("gimbal not available")
        }
    }

    function pressControl() {
        if (!shouldProcessClicks) {
            return
        }
        if (!_gimbalControllerSettings.ControlType.rawValue == 1) {
            return
        }
        sendRateTimer.start()
        screenXrateInitCoocked =  ( ( screenX / parent.width)  * 2) - 1
        screenYrateInitCoocked = -( ( screenY / parent.height) * 2) + 1
    }

    function releaseControl() {
        if (!shouldProcessClicks) {
            return
        }
        if (!_gimbalControllerSettings.ControlType.rawValue == 1) {
            return
        }
        sendRateTimer.stop()
        screenXrateInitCoocked = null
        screenYrateInitCoocked = null
        // _activeVehicle.gimbalController.gimbalOnScreenControl(0, 0, false, true, true)
    }

    Timer {
        id:             sendRateTimer
        interval:       100
        repeat:         true
        onTriggered: {
            if (rootItem._gimbalAvailable) {
                var xCoocked =  ( ( screenX / parent.width)  * 2) - 1
                var yCoocked = -( ( screenY / parent.height) * 2) + 1
                xCoocked -= screenXrateInitCoocked
                yCoocked -= screenYrateInitCoocked
                _gimbalController.gimbalOnScreenControl(xCoocked, yCoocked, false, true, true)
            } else {
                console.log("gimbal not available")
            }
        }
    }
}