message("Adding Custom Plugin")

#-- Version control
#   Major and minor versions are defined here (manually)

CUSTOM_QGC_VER_MAJOR = 4
CUSTOM_QGC_VER_MINOR = 4
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