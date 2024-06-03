message("Adding Custom Plugin")

#-- Version control
#   Major and minor versions are defined here (manually)

CUSTOM_QGC_VER_MAJOR = 0
CUSTOM_QGC_VER_MINOR = 0
CUSTOM_QGC_VER_FIRST_BUILD = 0

# Build number is automatic
# Uses the current branch. This way it works on any branch including build-server's PR branches
CUSTOM_QGC_VER_BUILD = $$system(git --git-dir ../.git rev-list $$GIT_BRANCH --first-parent --count)
win32 {
    CUSTOM_QGC_VER_BUILD = $$system("set /a $$CUSTOM_QGC_VER_BUILD - $$CUSTOM_QGC_VER_FIRST_BUILD")
} else {
    CUSTOM_QGC_VER_BUILD = $$system("echo $(($$CUSTOM_QGC_VER_BUILD - $$CUSTOM_QGC_VER_FIRST_BUILD))")
}
CUSTOM_QGC_VERSION = $${CUSTOM_QGC_VER_MAJOR}.$${CUSTOM_QGC_VER_MINOR}.$${CUSTOM_QGC_VER_BUILD}

DEFINES -= APP_VERSION_STR=\"\\\"$$APP_VERSION_STR\\\"\"
DEFINES += APP_VERSION_STR=\"\\\"$$CUSTOM_QGC_VERSION\\\"\"

message(Custom QGC Version: $${CUSTOM_QGC_VERSION})

# Branding

DEFINES += CUSTOMHEADER=\"\\\"CustomPlugin.h\\\"\"
DEFINES += CUSTOMCLASS=CustomPlugin

TARGET   = CustomQGroundControl
DEFINES += QGC_APPLICATION_NAME='"\\\"Custom QGroundControl\\\""'

DEFINES += QGC_ORG_NAME=\"\\\"qgroundcontrol.org\\\"\"
DEFINES += QGC_ORG_DOMAIN=\"\\\"org.qgroundcontrol\\\"\"

QGC_APP_NAME        = "Custom QGroundControl"
QGC_BINARY_NAME     = "CustomQGroundControl"
QGC_ORG_NAME        = "Custom"
QGC_ORG_DOMAIN      = "org.custom"
QGC_ANDROID_PACKAGE = "org.custom.qgroundcontrol"
QGC_APP_DESCRIPTION = "Custom QGroundControl"
QGC_APP_COPYRIGHT   = "Copyright (C) 2020 QGroundControl Development Team. All rights reserved."

# Our own, custom resources
RESOURCES += \
    $$PWD/custom.qrc

QML_IMPORT_PATH += \
   $$PWD/res

# Our own, custom sources
SOURCES += \
    $$PWD/src/CustomPlugin.cc \

HEADERS += \
    $$PWD/src/CustomPlugin.h \

INCLUDEPATH += \
    $$PWD/src \

#-------------------------------------------------------------------------------------
# Custom Firmware/AutoPilot Plugin

# INCLUDEPATH += \
#     $$PWD/src/FirmwarePlugin/PX4 \
#     $$PWD/src/AutoPilotPlugin/PX4 \
#     $$PWD/src/FirmwarePlugin/AP \
#     $$PWD/src/AutoPilotPlugin/AP
# 
# HEADERS+= \
#     $$PWD/src/AutoPilotPlugin/PX4/CustomPX4AutoPilotPlugin.h \
#     $$PWD/src/FirmwarePlugin/PX4/CustomPX4FirmwarePlugin.h \
#     $$PWD/src/FirmwarePlugin/PX4/CustomPX4FirmwarePluginFactory.h \
#     $$PWD/src/AutoPilotPlugin/AP/CustomAPAutoPilotPlugin.h \
#     $$PWD/src/FirmwarePlugin/AP/CustomAPFirmwarePlugin.h \
#     $$PWD/src/FirmwarePlugin/AP/CustomAPFirmwarePluginFactory.h
# 
# SOURCES += \
#     $$PWD/src/AutoPilotPlugin/PX4/CustomPX4AutoPilotPlugin.cc \
#     $$PWD/src/FirmwarePlugin/PX4/CustomPX4FirmwarePlugin.cc \
#     $$PWD/src/FirmwarePlugin/PX4/CustomPX4FirmwarePluginFactory.cc \
#     $$PWD/src/AutoPilotPlugin/AP/CustomAPAutoPilotPlugin.cc \
#     $$PWD/src/FirmwarePlugin/AP/CustomAPFirmwarePlugin.cc \
#     $$PWD/src/FirmwarePlugin/AP/CustomAPFirmwarePluginFactory.cc \

