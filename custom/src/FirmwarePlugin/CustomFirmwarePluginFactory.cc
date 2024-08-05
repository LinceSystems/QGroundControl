#include "CustomFirmwarePluginFactory.h"
#include "CustomAPCopterFirmwarePlugin.h"
#include "CustomAPPlaneFirmwarePlugin.h"
#include "APM/ArduRoverFirmwarePlugin.h"
#include "APM/ArduSubFirmwarePlugin.h"

CustomFirmwarePluginFactory CustomFirmwarePluginFactoryImp;

CustomFirmwarePluginFactory::CustomFirmwarePluginFactory()
    : _arduCopterPluginInstance(nullptr)
    , _arduPlanePluginInstance(nullptr)
    , _arduRoverPluginInstance(nullptr)
    , _arduSubPluginInstance(nullptr)
{

}

QList<QGCMAVLink::FirmwareClass_t> CustomFirmwarePluginFactory::supportedFirmwareClasses() const
{
    QList<QGCMAVLink::FirmwareClass_t> firmwareClasses;
    firmwareClasses.append(QGCMAVLink::FirmwareClassArduPilot);
    return firmwareClasses;
}

QList<QGCMAVLink::VehicleClass_t> CustomFirmwarePluginFactory::supportedVehicleClasses(void) const
{
    QList<QGCMAVLink::VehicleClass_t> vehicleClasses;
    vehicleClasses.append(QGCMAVLink::VehicleClassMultiRotor);
    vehicleClasses.append(QGCMAVLink::VehicleClassFixedWing);
    vehicleClasses.append(QGCMAVLink::VehicleClassVTOL);
    return vehicleClasses;
}

FirmwarePlugin* CustomFirmwarePluginFactory::firmwarePluginForAutopilot(MAV_AUTOPILOT autopilotType, MAV_TYPE vehicleType)
{
    if (autopilotType == MAV_AUTOPILOT_ARDUPILOTMEGA) {
        switch (vehicleType) {
        case MAV_TYPE_QUADROTOR:
        case MAV_TYPE_HEXAROTOR:
        case MAV_TYPE_OCTOROTOR:
        case MAV_TYPE_TRICOPTER:
        case MAV_TYPE_COAXIAL:
        case MAV_TYPE_HELICOPTER:
            if (!_arduCopterPluginInstance) {
                _arduCopterPluginInstance = new CustomAPCopterFirmwarePlugin;
            }
            return _arduCopterPluginInstance;
        case MAV_TYPE_VTOL_TAILSITTER_DUOROTOR:
        case MAV_TYPE_VTOL_TAILSITTER_QUADROTOR:
        case MAV_TYPE_VTOL_TILTROTOR:
        case MAV_TYPE_VTOL_FIXEDROTOR:
        case MAV_TYPE_VTOL_TAILSITTER:
        case MAV_TYPE_VTOL_TILTWING:
        case MAV_TYPE_VTOL_RESERVED5:
        case MAV_TYPE_FIXED_WING:
            if (!_arduPlanePluginInstance) {
                _arduPlanePluginInstance = new CustomAPPlaneFirmwarePlugin;
            }
            return _arduPlanePluginInstance;
        case MAV_TYPE_GROUND_ROVER:
        case MAV_TYPE_SURFACE_BOAT:
            if (!_arduRoverPluginInstance) {
                _arduRoverPluginInstance = new ArduRoverFirmwarePlugin;
            }
            return _arduRoverPluginInstance;
        case MAV_TYPE_SUBMARINE:
            if (!_arduSubPluginInstance) {
                _arduSubPluginInstance = new ArduSubFirmwarePlugin;
            }
            return _arduSubPluginInstance;
        default:
            break;
        }
    }

    return nullptr;
}
