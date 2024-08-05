#pragma once

#include "FirmwarePlugin.h"
#include "ArduCopterFirmwarePlugin.h"

class CustomAPCopterFirmwarePlugin : public ArduCopterFirmwarePlugin
{
    Q_OBJECT
public:
    CustomAPCopterFirmwarePlugin();

    // FirmwarePlugin overrides
    AutoPilotPlugin*    autopilotPlugin (Vehicle* vehicle) override;
    const QVariantList& toolIndicators  (const Vehicle* vehicle) override;
    
private:
    QVariantList _toolIndicatorList;
};
