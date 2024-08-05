#pragma once

#include "FirmwarePlugin.h"
#include "ArduPlaneFirmwarePlugin.h"

class CustomAPPlaneFirmwarePlugin : public ArduPlaneFirmwarePlugin
{
    Q_OBJECT
public:
    CustomAPPlaneFirmwarePlugin();

    // FirmwarePlugin overrides
    AutoPilotPlugin*    autopilotPlugin (Vehicle* vehicle) override;
    const QVariantList& toolIndicators  (const Vehicle* vehicle) override;
    
private:
    QVariantList _toolIndicatorList;
};
