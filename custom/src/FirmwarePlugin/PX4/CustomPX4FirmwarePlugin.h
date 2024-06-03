#pragma once

#include "FirmwarePlugin.h"
#include "PX4FirmwarePlugin.h"

class CustomCameraManager;

class CustomPX4FirmwarePlugin : public PX4FirmwarePlugin
{
    Q_OBJECT
public:
    CustomPX4FirmwarePlugin();

    // FirmwarePlugin overrides
    AutoPilotPlugin*    autopilotPlugin (Vehicle* vehicle) final;
    const QVariantList& toolIndicators  (const Vehicle* vehicle) final;
    bool                hasGimbal       (Vehicle* vehicle, bool& rollSupported, bool& pitchSupported, bool& yawSupported) final;

private:
    QVariantList _toolIndicatorList;
};
