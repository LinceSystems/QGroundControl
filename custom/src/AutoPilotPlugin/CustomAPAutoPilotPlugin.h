#pragma once

#include "APMAutoPilotPlugin.h"
#include "Vehicle.h"

class CustomAPAutoPilotPlugin : public APMAutoPilotPlugin
{
    Q_OBJECT
public:
    CustomAPAutoPilotPlugin(Vehicle* vehicle, QObject* parent);
    ~CustomAPAutoPilotPlugin();

    // const QVariantList& vehicleComponents() final;

signals:
    void safeRtlTimeChanged();
    void toHomeTimeChanged();

private slots:
    void         _advancedChanged        (bool advanced);

private:
    void         _customParamsLoadedProcedure(bool ready);
    void         _mavlinkMessageReceived(mavlink_message_t message);

private:
    QVariantList _components;
};
