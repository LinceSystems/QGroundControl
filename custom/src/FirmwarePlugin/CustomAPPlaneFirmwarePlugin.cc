#include "CustomAPPlaneFirmwarePlugin.h"
#include "CustomAPAutoPilotPlugin.h"
#include "QGCApplication.h"

//-----------------------------------------------------------------------------
CustomAPPlaneFirmwarePlugin::CustomAPPlaneFirmwarePlugin()
{
    setSupportedModes({
        APMPlaneMode(APMPlaneMode::MANUAL,           false),
        APMPlaneMode(APMPlaneMode::CIRCLE,           false),
        APMPlaneMode(APMPlaneMode::STABILIZE,        false),
        APMPlaneMode(APMPlaneMode::TRAINING,         false),
        APMPlaneMode(APMPlaneMode::ACRO,             false),
        APMPlaneMode(APMPlaneMode::FLY_BY_WIRE_A,    true),
        APMPlaneMode(APMPlaneMode::FLY_BY_WIRE_B,    false),
        APMPlaneMode(APMPlaneMode::CRUISE,           false),
        APMPlaneMode(APMPlaneMode::AUTOTUNE,         false),
        APMPlaneMode(APMPlaneMode::AUTO,             true),
        APMPlaneMode(APMPlaneMode::RTL,              true),
        APMPlaneMode(APMPlaneMode::LOITER,           true),
        APMPlaneMode(APMPlaneMode::TAKEOFF,          false),
        APMPlaneMode(APMPlaneMode::AVOID_ADSB,       false),
        APMPlaneMode(APMPlaneMode::GUIDED,           false),
        APMPlaneMode(APMPlaneMode::INITIALIZING,     false),
        APMPlaneMode(APMPlaneMode::QSTABILIZE,       true),
        APMPlaneMode(APMPlaneMode::QHOVER,           true),
        APMPlaneMode(APMPlaneMode::QLOITER,          true),
        APMPlaneMode(APMPlaneMode::QLAND,            true),
        APMPlaneMode(APMPlaneMode::QRTL,             true),
        APMPlaneMode(APMPlaneMode::QAUTOTUNE,        false),
        APMPlaneMode(APMPlaneMode::QACRO,            false),
        APMPlaneMode(APMPlaneMode::THERMAL,          false),
    });
}

//-----------------------------------------------------------------------------
AutoPilotPlugin* CustomAPPlaneFirmwarePlugin::autopilotPlugin(Vehicle* vehicle)
{
    return new CustomAPAutoPilotPlugin(vehicle, vehicle);
}

const QVariantList& CustomAPPlaneFirmwarePlugin::toolIndicators(const Vehicle* vehicle)
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
