#include "CustomPX4FirmwarePluginFactory.h"
#include "CustomPX4FirmwarePlugin.h"

CustomPX4FirmwarePluginFactory CustomPX4FirmwarePluginFactoryImp;

CustomPX4FirmwarePluginFactory::CustomPX4FirmwarePluginFactory()
    : _pluginInstance(nullptr)
{

}

QList<QGCMAVLink::FirmwareClass_t> CustomPX4FirmwarePluginFactory::supportedFirmwareClasses() const
{
    QList<QGCMAVLink::FirmwareClass_t> firmwareClasses;
    firmwareClasses.append(QGCMAVLink::FirmwareClassPX4);
    return firmwareClasses;
}

QList<QGCMAVLink::VehicleClass_t> CustomPX4FirmwarePluginFactory::supportedVehicleClasses(void) const
{
    QList<QGCMAVLink::VehicleClass_t> vehicleClasses;
    vehicleClasses.append(QGCMAVLink::VehicleClassMultiRotor);
    return vehicleClasses;
}

FirmwarePlugin* CustomPX4FirmwarePluginFactory::firmwarePluginForAutopilot(MAV_AUTOPILOT autopilotType, MAV_TYPE /*vehicleType*/)
{
    if (autopilotType == MAV_AUTOPILOT_PX4) {
        if (!_pluginInstance) {
            _pluginInstance = new CustomPX4FirmwarePlugin;
        }
        return _pluginInstance;
    }
    return nullptr;
}
