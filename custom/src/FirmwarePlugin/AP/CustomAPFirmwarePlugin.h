#pragma once

#include "FirmwarePlugin.h"
#include "ArduCopterFirmwarePlugin.h"

class CustomAPFirmwarePlugin : public ArduCopterFirmwarePlugin
{
    Q_OBJECT
public:
    CustomAPFirmwarePlugin();
};
