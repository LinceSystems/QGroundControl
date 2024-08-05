#include "CustomAPAutoPilotPlugin.h"
#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "ParameterManager.h"
#include "SettingsManager.h"

CustomAPAutoPilotPlugin::CustomAPAutoPilotPlugin(Vehicle* vehicle, QObject* parent)
    : APMAutoPilotPlugin(vehicle, parent)
{
    // Whenever we go on/out of advanced mode the available list of settings pages will change
    connect(qgcApp()->toolbox()->corePlugin(), &QGCCorePlugin::showAdvancedUIChanged, this, &CustomAPAutoPilotPlugin::_advancedChanged);

    // Connect parameters ready signal to our _customParamsLoadedProcedure
    connect(qgcApp()->toolbox()->multiVehicleManager(), &MultiVehicleManager::parameterReadyVehicleAvailableChanged, this, &CustomAPAutoPilotPlugin::_customParamsLoadedProcedure);

    // Connect mavlink message received signal
    connect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &CustomAPAutoPilotPlugin::_mavlinkMessageReceived);
}

CustomAPAutoPilotPlugin::~CustomAPAutoPilotPlugin()
{

}

// This signals that when Advanced Mode changes the list of Vehicle Settings page also changed
void CustomAPAutoPilotPlugin::_advancedChanged(bool)
{
    _components.clear();
    emit vehicleComponentsChanged();
}


void CustomAPAutoPilotPlugin::_customParamsLoadedProcedure(bool ready)
{
    // If parameters are not ready ( like in a vehicle disconnection, or just not ready ) don't proceed, the rest of this depends on it
    if (!ready) {
        return;
    }
}

void CustomAPAutoPilotPlugin::_mavlinkMessageReceived(mavlink_message_t message)
{

}
