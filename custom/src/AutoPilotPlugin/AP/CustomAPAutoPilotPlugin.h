#pragma once

#include "APMAutoPilotPlugin.h"
#include "Vehicle.h"

class CustomAPAutoPilotPlugin : public APMAutoPilotPlugin
{
    Q_OBJECT
public:
    CustomAPAutoPilotPlugin(Vehicle* vehicle, QObject* parent);
};
