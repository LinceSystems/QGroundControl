#pragma once

#include "FirmwarePlugin.h"

class CustomPX4FirmwarePlugin;

/// This custom implementation of FirmwarePluginFactory creates a custom build which only supports
/// PX4 Pro firmware running on a multi-rotor vehicle. This is turn removes portions of the QGC UI
/// related to other firmware and vehicle types. This creating a more simplified UI for a specific
/// type of vehicle.
class CustomPX4FirmwarePluginFactory : public FirmwarePluginFactory
{
    Q_OBJECT
public:
    CustomPX4FirmwarePluginFactory();
    QList<QGCMAVLink::FirmwareClass_t>  supportedFirmwareClasses(void) const final;
    QList<QGCMAVLink::VehicleClass_t>   supportedVehicleClasses(void) const final;
    FirmwarePlugin*                     firmwarePluginForAutopilot  (MAV_AUTOPILOT autopilotType, MAV_TYPE vehicleType) final;

private:
    CustomPX4FirmwarePlugin*   _pluginInstance;
};

extern CustomPX4FirmwarePluginFactory CustomPX4FirmwarePluginFactoryImp;
