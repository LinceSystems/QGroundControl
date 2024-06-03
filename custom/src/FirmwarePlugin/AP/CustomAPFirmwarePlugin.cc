#include "CustomAPFirmwarePlugin.h"
#include "CustomAPAutoPilotPlugin.h"

//-----------------------------------------------------------------------------
CustomAPFirmwarePlugin::CustomAPFirmwarePlugin()
{

}

//-----------------------------------------------------------------------------
AutoPilotPlugin* CustomAPFirmwarePlugin::autopilotPlugin(Vehicle* vehicle)
{
    return new CustomAPAutoPilotPlugin(vehicle, vehicle);
}