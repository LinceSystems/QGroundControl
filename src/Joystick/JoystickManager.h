/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

/// @file
/// @brief  Joystick Manager

#pragma once

#include "QGCLoggingCategory.h"
#include "Joystick.h"
#include "MultiVehicleManager.h"
#include "QGCToolbox.h"

#include <QVariantList>

Q_DECLARE_LOGGING_CATEGORY(JoystickManagerLog)

/// Joystick Manager
class JoystickManager : public QGCTool
{
    Q_OBJECT

public:
    JoystickManager(QGCApplication* app, QGCToolbox* toolbox);
    ~JoystickManager();

    Q_PROPERTY(QVariantList joysticks READ joysticks NOTIFY availableJoysticksChanged)
    Q_PROPERTY(QStringList  joystickNames READ joystickNames NOTIFY availableJoysticksChanged)

    Q_PROPERTY(Joystick* activeJoystick READ activeJoystick WRITE setActiveJoystick NOTIFY activeJoystickChanged)
    Q_PROPERTY(QString activeJoystickName READ activeJoystickName WRITE setActiveJoystickName NOTIFY activeJoystickNameChanged)

    Q_PROPERTY(Joystick* activeJoystickSecondary READ activeJoystickSecondary WRITE setActiveJoystickSecondary NOTIFY activeJoystickSecondaryChanged)
    Q_PROPERTY(QString activeJoystickSecondaryName READ activeJoystickSecondaryName WRITE setActiveJoystickSecondaryName NOTIFY activeJoystickSecondaryNameChanged)

    Q_PROPERTY(bool multiJoystickConfigOk READ multiJoystickConfigOk NOTIFY multiJoystickConfigOkChanged)
    
    /// List of available joysticks
    QVariantList joysticks();
    /// List of available joystick names
    QStringList joystickNames(void);

    /// Get active joystick
    Joystick* activeJoystick(void);
    /// Set active joystick
    void setActiveJoystick(Joystick* joystick);

    /// Get active joystickSecondary
    Joystick* activeJoystickSecondary(void);
    /// Set active joystickSecondary
    void setActiveJoystickSecondary(Joystick* joystick);

    QString activeJoystickName(void);
    bool setActiveJoystickName(const QString& name);

    QString activeJoystickSecondaryName(void);
    bool setActiveJoystickSecondaryName(const QString& name);

    bool multiJoystickConfigOk() { return _multiJoystickConfigOk; }

    void restartJoystickCheckTimer(void);

    // Override from QGCTool
    virtual void setToolbox(QGCToolbox *toolbox);

public slots:
    void init();
    void evaluateMultiJoystickConfigOk();

signals:
    void activeJoystickChanged(Joystick* joystick);
    void activeJoystickNameChanged(const QString& name);
    void activeJoystickSecondaryChanged(Joystick* joystick);
    void activeJoystickSecondaryNameChanged(const QString& name);
    void availableJoysticksChanged(void);
    void updateAvailableJoysticksSignal();
    void multiJoystickConfigOkChanged();

private slots:
    void _updateAvailableJoysticks(void);

private:
    void _setActiveJoysticksFromSettings(void);

private:
    Joystick*                   _activeJoystick;
    Joystick*                   _activeJoystickSecondary;
    QMap<QString, Joystick*>    _name2JoystickMap;
    MultiVehicleManager*        _multiVehicleManager;

    static const char * _settingsGroup;
    static const char * _settingsKeyActiveJoystick;
    static const char * _settingsKeyActiveJoystickSecondary;

    int _joystickCheckTimerCounter;
    QTimer _joystickCheckTimer;

    bool _multiJoystickConfigOk = false;
};
