#include "CustomAPFirmwarePluginFactory.h"
#include "CustomAPFirmwarePlugin.h"

CustomAPFirmwarePluginFactory CustomAPFirmwarePluginFactoryImp;

CustomAPFirmwarePluginFactory::CustomAPFirmwarePluginFactory()
    : _pluginInstance(nullptr)
{

}

QList<QGCMAVLink::FirmwareClass_t> CustomAPFirmwarePluginFactory::supportedFirmwareClasses() const
{
    QList<QGCMAVLink::FirmwareClass_t> firmwareClasses;
    firmwareClasses.append(QGCMAVLink::FirmwareClassArduPilot);
    return firmwareClasses;
}

QList<QGCMAVLink::VehicleClass_t> CustomAPFirmwarePluginFactory::supportedVehicleClasses(void) const
{
    QList<QGCMAVLink::VehicleClass_t> vehicleClasses;
    vehicleClasses.append(QGCMAVLink::VehicleClassMultiRotor);
    return vehicleClasses;
}

FirmwarePlugin* CustomAPFirmwarePluginFactory::firmwarePluginForAutopilot(MAV_AUTOPILOT autopilotType, MAV_TYPE /*vehicleType*/)
{
    if (autopilotType == MAV_AUTOPILOT_ARDUPILOTMEGA) {
        if (!_pluginInstance) {
            _pluginInstance = new CustomAPFirmwarePlugin;
        }
        return _pluginInstance;
    }
    return nullptr;
}
