#pragma once

#include "FirmwarePlugin.h"

class CustomAPCopterFirmwarePlugin;
class CustomAPPlaneFirmwarePlugin;
class ArduRoverFirmwarePlugin;
class ArduSubFirmwarePlugin;

/// This custom implementation of FirmwarePluginFactory creates a custom build which only supports
/// Ardupilot firmware, for Copters and Vtols or fixed wings. This is turn removes portions of the QGC UI
/// related to other firmware and vehicle types. This creating a more simplified UI for a specific
/// type of vehicle.

/// The way this works is that firmwarepluginfactory, on its constructor, calls:

//    FirmwarePluginFactoryRegister::instance()->registerPluginFactory(this);

// and firmwarePluginManager, on _findPluginFactory, iterates through all the registered plugins in
// FirmwarePluginFactoryRegister, which is global and singleton, and returns the first one compatible
// with the specific firmware type ( px4 or apm ). As the files in custom folder are included, built
// prior to the standard firmwarepluginfactory, they will always have a previous id than the standard
// ones, as its registration will always be performed prior to the standard ones, so custom 
// firmwarepluginfactories will always override standard ones for same vehicle type


class CustomFirmwarePluginFactory : public FirmwarePluginFactory
{
    Q_OBJECT
public:
    CustomFirmwarePluginFactory();
    QList<QGCMAVLink::FirmwareClass_t>  supportedFirmwareClasses(void) const final;
    QList<QGCMAVLink::VehicleClass_t>   supportedVehicleClasses(void) const final;
    FirmwarePlugin*                     firmwarePluginForAutopilot  (MAV_AUTOPILOT autopilotType, MAV_TYPE vehicleType) final;

private:
    CustomAPCopterFirmwarePlugin*  _arduCopterPluginInstance;
    CustomAPPlaneFirmwarePlugin*   _arduPlanePluginInstance;
    ArduRoverFirmwarePlugin*       _arduRoverPluginInstance;
    ArduSubFirmwarePlugin*         _arduSubPluginInstance;
};

extern CustomFirmwarePluginFactory CustomFirmwarePluginFactoryImp;
