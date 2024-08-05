message("Adding Lince Custom Plugin")

#-- Version control
CUSTOM_QGC_VERSION = "$${GIT_DESCRIBE}"

DEFINES -= APP_VERSION_STR=\"\\\"$$APP_VERSION_STR\\\"\"
DEFINES += APP_VERSION_STR=\"\\\"$$CUSTOM_QGC_VERSION\\\"\"

message(Custom QGC Version: $${CUSTOM_QGC_VERSION})

# plugin factory, only use our custom arducopter plugin factory, based on standard apm plugin
CONFIG  += QGC_DISABLE_APM_PLUGIN_FACTORY

# disable px4 plugin factory altogether
CONFIG  += QGC_DISABLE_PX4_PLUGIN_FACTORY

# Windows Branding icon
WindowsBuild {
    RC_ICONS = $$PWD/deploy/windows/CustomWindowsIcon.ico
    QGC_INSTALLER_HEADER_BITMAP = "$$PWD\\deploy\\windows\\installheader.bmp"
    QGC_INSTALLER_ICON = "$$PWD\\deploy\\windows\\CustomWindowsIcon.ico"
}

# Mac Branding icon
MacBuild {
    ICON = $$PWD/deploy/macos/macx.icns
}

# Branding

DEFINES += CUSTOMHEADER=\"\\\"CustomPlugin.h\\\"\"
DEFINES += CUSTOMCLASS=CustomPlugin

TARGET   = Lince-GroundControl
DEFINES += QGC_APPLICATION_NAME='"\\\"Lince-GroundControl\\\""'

DEFINES += QGC_ORG_NAME=\"\\\"lincesystems\\\"\"
DEFINES += QGC_ORG_DOMAIN=\"\\\"org.lincesystems\\\"\"

QGC_APP_NAME        = "Lince-GroundControl"
QGC_BINARY_NAME     = "Lince-GroundControl"
QGC_ORG_NAME        = "lincesystems"
QGC_ORG_DOMAIN      = "org.lincesystems"
QGC_ANDROID_PACKAGE = "org.lincesystems.lince_groundcontrol"
QGC_APP_DESCRIPTION = "Lince-GroundControl"
QGC_APP_COPYRIGHT   = "Copyright (C) 2024 Lince Systems. All rights reserved."

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

#--------------------------------------------------------------------------------------
# Custom Firmware/AutoPilot Plugin

INCLUDEPATH += \
    $$PWD/src/FirmwarePlugin \
    $$PWD/src/AutoPilotPlugin

HEADERS+= \
    $$PWD/src/AutoPilotPlugin/CustomAPAutoPilotPlugin.h \
    $$PWD/src/FirmwarePlugin/CustomAPCopterFirmwarePlugin.h \
    $$PWD/src/FirmwarePlugin/CustomAPPlaneFirmwarePlugin.h \
    $$PWD/src/FirmwarePlugin/CustomFirmwarePluginFactory.h \

SOURCES += \
    $$PWD/src/AutoPilotPlugin/CustomAPAutoPilotPlugin.cc \
    $$PWD/src/FirmwarePlugin/CustomAPCopterFirmwarePlugin.cc \
    $$PWD/src/FirmwarePlugin/CustomAPPlaneFirmwarePlugin.cc \
    $$PWD/src/FirmwarePlugin/CustomFirmwarePluginFactory.cc \