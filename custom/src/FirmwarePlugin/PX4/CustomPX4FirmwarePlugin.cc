#include "CustomPX4FirmwarePlugin.h"
#include "CustomPX4AutoPilotPlugin.h"

//-----------------------------------------------------------------------------
CustomPX4FirmwarePlugin::CustomPX4FirmwarePlugin()
{

}

//-----------------------------------------------------------------------------
AutoPilotPlugin* CustomPX4FirmwarePlugin::autopilotPlugin(Vehicle* vehicle)
{
    return new CustomPX4AutoPilotPlugin(vehicle, vehicle);
}