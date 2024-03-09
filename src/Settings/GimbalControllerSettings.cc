/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "GimbalControllerSettings.h"
#include "QGCApplication.h"

#include <QQmlEngine>
#include <QtQml>
#include <QVariantList>

#ifndef QGC_DISABLE_UVC
#include <QCameraInfo>
#endif

DECLARE_SETTINGGROUP(GimbalController, "GimbalController")
{
    qmlRegisterUncreatableType<GimbalControllerSettings>("QGroundControl.SettingsManager", 1, 0, "GimbalControllerSettings", "Reference only");
}

DECLARE_SETTINGSFACT(GimbalControllerSettings, EnableOnScreenControl)
DECLARE_SETTINGSFACT(GimbalControllerSettings, ControlType)
DECLARE_SETTINGSFACT(GimbalControllerSettings, CameraVFov)
DECLARE_SETTINGSFACT(GimbalControllerSettings, CameraHFov)
DECLARE_SETTINGSFACT(GimbalControllerSettings, CameraSlideSpeed)