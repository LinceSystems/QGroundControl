#pragma once

#include "PX4AutoPilotPlugin.h"
#include "Vehicle.h"

class CustomPX4AutoPilotPlugin : public PX4AutoPilotPlugin
{
    Q_OBJECT
public:
    CustomPX4AutoPilotPlugin(Vehicle* vehicle, QObject* parent);
};
