#include "CustomAPCopterFirmwarePlugin.h"
#include "CustomAPAutoPilotPlugin.h"
#include "QGCApplication.h"

//-----------------------------------------------------------------------------
CustomAPCopterFirmwarePlugin::CustomAPCopterFirmwarePlugin()
{
    setSupportedModes({
        APMCopterMode(APMCopterMode::STABILIZE,     false),
        APMCopterMode(APMCopterMode::ACRO,          false),
        APMCopterMode(APMCopterMode::ALT_HOLD,      true),
        APMCopterMode(APMCopterMode::AUTO,          true),
        APMCopterMode(APMCopterMode::GUIDED,        true),
        APMCopterMode(APMCopterMode::LOITER,        true),
        APMCopterMode(APMCopterMode::RTL,           true),
        APMCopterMode(APMCopterMode::CIRCLE,        false),
        APMCopterMode(APMCopterMode::LAND,          true),
        APMCopterMode(APMCopterMode::DRIFT,         false),
        APMCopterMode(APMCopterMode::SPORT,         false),
        APMCopterMode(APMCopterMode::FLIP,          false),
        APMCopterMode(APMCopterMode::AUTOTUNE,      false),
        APMCopterMode(APMCopterMode::POS_HOLD,      false),
        APMCopterMode(APMCopterMode::BRAKE,         false),
        APMCopterMode(APMCopterMode::THROW,         false),
        APMCopterMode(APMCopterMode::AVOID_ADSB,    false),
        APMCopterMode(APMCopterMode::GUIDED_NOGPS,  false),
        APMCopterMode(APMCopterMode::SMART_RTL,     false),
        APMCopterMode(APMCopterMode::FLOWHOLD,      false),
        APMCopterMode(APMCopterMode::FOLLOW,        false),
        APMCopterMode(APMCopterMode::ZIGZAG,        false),
        APMCopterMode(APMCopterMode::SYSTEMID,      false),
        APMCopterMode(APMCopterMode::AUTOROTATE,    false),
        APMCopterMode(APMCopterMode::AUTO_RTL,      false),
        APMCopterMode(APMCopterMode::TURTLE,        false)
    });
}

//-----------------------------------------------------------------------------
AutoPilotPlugin* CustomAPCopterFirmwarePlugin::autopilotPlugin(Vehicle* vehicle)
{
    return new CustomAPAutoPilotPlugin(vehicle, vehicle);
}

const QVariantList& CustomAPCopterFirmwarePlugin::toolIndicators(const Vehicle* vehicle)
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