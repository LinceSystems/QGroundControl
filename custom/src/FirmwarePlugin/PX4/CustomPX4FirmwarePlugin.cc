#include "CustomPX4FirmwarePlugin.h"
#include "CustomPX4AutoPilotPlugin.h"

//-----------------------------------------------------------------------------
CustomPX4FirmwarePlugin::CustomPX4FirmwarePlugin()
{
    for (int i = 0; i < _flightModeInfoList.count(); i++) {
        FlightModeInfo_t& info = _flightModeInfoList[i];
        //-- Narrow the flight mode options to only these
        if (info.name != _holdFlightMode && info.name != _rtlFlightMode && info.name != _missionFlightMode) {
            // No other flight modes can be set
            info.canBeSet = false;
        }
    }
}

//-----------------------------------------------------------------------------
AutoPilotPlugin* CustomPX4FirmwarePlugin::autopilotPlugin(Vehicle* vehicle)
{
    return new CustomPX4AutoPilotPlugin(vehicle, vehicle);
}

const QVariantList& CustomPX4FirmwarePlugin::toolIndicators(const Vehicle* vehicle)
{
    if (_toolIndicatorList.size() == 0) {
        // First call the base class to get the standard QGC list. This way we are guaranteed to always get
        // any new toolbar indicators which are added upstream in our custom build.
        _toolIndicatorList = FirmwarePlugin::toolIndicators(vehicle);
        // Then specifically remove the RC RSSI indicator.
        _toolIndicatorList.removeOne(QVariant::fromValue(QUrl::fromUserInput("qrc:/toolbar/RCRSSIIndicator.qml")));
    }
    return _toolIndicatorList;
}

// Tells QGC that your vehicle has a gimbal on it. This will in turn cause thing like gimbal commands to point
// the camera straight down for surveys to be automatically added to Plans.
bool CustomPX4FirmwarePlugin::hasGimbal(Vehicle* /*vehicle*/, bool& rollSupported, bool& pitchSupported, bool& yawSupported)
{
    rollSupported = false;
    pitchSupported = true;
    yawSupported = true;

    return true;
}
