/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#include "JoystickManager.h"
#include "QGCApplication.h"

#include <QQmlEngine>

#ifndef __mobile__
    #include "JoystickSDL.h"
    #define __sdljoystick__
#endif

#ifdef __android__
    #include "JoystickAndroid.h"
#endif

QGC_LOGGING_CATEGORY(JoystickManagerLog, "JoystickManagerLog")

const char * JoystickManager::_settingsGroup =                       "JoystickManager";
const char * JoystickManager::_settingsKeyActiveJoystick =           "ActiveJoystick";
const char * JoystickManager::_settingsKeyActiveJoystickSecondary =  "ActiveJoystickSecondary";

JoystickManager::JoystickManager(QGCApplication* app, QGCToolbox* toolbox)
    : QGCTool(app, toolbox)
    , _activeJoystick(nullptr)
    , _activeJoystickSecondary(nullptr)
    , _multiVehicleManager(nullptr)
{
}

JoystickManager::~JoystickManager() {
    QMap<QString, Joystick*>::iterator i;
    for (i = _name2JoystickMap.begin(); i != _name2JoystickMap.end(); ++i) {
        qCDebug(JoystickManagerLog) << "Releasing joystick:" << i.key();
        i.value()->stop();
        delete i.value();
    }
    qDebug() << "Done";
}

void JoystickManager::setToolbox(QGCToolbox *toolbox)
{
    QGCTool::setToolbox(toolbox);

    _multiVehicleManager = _toolbox->multiVehicleManager();

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

void JoystickManager::init() {
#ifdef __sdljoystick__
    if (!JoystickSDL::init()) {
        return;
    }
    _setActiveJoysticksFromSettings();
#elif defined(__android__)
    if (!JoystickAndroid::init(this)) {
        return;
    }
    connect(this, &JoystickManager::updateAvailableJoysticksSignal, this, &JoystickManager::restartJoystickCheckTimer);
#endif
    connect(&_joystickCheckTimer, &QTimer::timeout, this, &JoystickManager::_updateAvailableJoysticks);
    _joystickCheckTimerCounter = 5;
    _joystickCheckTimer.start(1000);
}

void JoystickManager::_setActiveJoysticksFromSettings(void)
{
    QMap<QString,Joystick*> newMap;

#ifdef __sdljoystick__
    // Get the latest joystick mapping
    newMap = JoystickSDL::discover(_multiVehicleManager);
#elif defined(__android__)
    newMap = JoystickAndroid::discover(_multiVehicleManager);
#endif

    if (_activeJoystick && !newMap.contains(_activeJoystick->name())) {
        qCDebug(JoystickManagerLog) << "Active joystick removed";
        setActiveJoystick(nullptr);
    }

    if (_activeJoystickSecondary && !newMap.contains(_activeJoystickSecondary->name())) {
        qCDebug(JoystickManagerLog) << "Active joystick secondary removed";
        setActiveJoystickSecondary(nullptr);
    }

    // Check to see if our current mapping contains any joysticks that are not in the new mapping
    // If so, those joysticks have been unplugged, and need to be cleaned up
    QMap<QString, Joystick*>::iterator i;
    for (i = _name2JoystickMap.begin(); i != _name2JoystickMap.end(); ++i) {
        if (!newMap.contains(i.key())) {
            qCDebug(JoystickManagerLog) << "Releasing joystick:" << i.key();
            i.value()->stopPolling();
            i.value()->wait(1000);
            i.value()->deleteLater();
        }
    }

    _name2JoystickMap = newMap;
    emit availableJoysticksChanged();

    if (!_name2JoystickMap.count()) {
        setActiveJoystick(nullptr);
        setActiveJoystickSecondary(nullptr);
        return;
    }

    QSettings settings;
    settings.beginGroup(_settingsGroup);

    // First read the settings for active joystick
    QString name = settings.value(_settingsKeyActiveJoystick).toString();
    qCDebug(JoystickManagerLog) << "Checking if we have primary joystick in settings ...";
    // If we don't have settings ( first boot, or never before a joystick was connected ), use first in the list
    if (name.isEmpty()) {
        qCDebug(JoystickManagerLog) << "Primary joystick not set in settings, using first in the connected devices list: " << _name2JoystickMap.first()->name();
        name = _name2JoystickMap.first()->name();
    // If se have it in settings, try to use that instead
    } else {
        qCDebug(JoystickManagerLog) << "Primary joystick present in settings, using it: " << name;
    }

    qCDebug(JoystickManagerLog) << "About to set Active joystick to: " << name << ", is it connected? Available joysticks: " << _name2JoystickMap.contains(name);
    // We would be here if the primary joystick we had in settings is connected, or if we didn't have any joystick in settings but we have one connected now
    if (_name2JoystickMap.contains(name)) { 
        qCDebug(JoystickManagerLog) << ("It is connected, setting active Joystick to: " + name);
        setActiveJoystick(_name2JoystickMap.value(name));
    // We would be here if the primary joystick we had in settings is not connected
    } else {
        qCDebug(JoystickManagerLog) << ("It is not connected, setting to first in the list: " + _name2JoystickMap.first()->name());
        setActiveJoystick(_name2JoystickMap.first());
        // We move the setting of the previous Primary to secondary. If for some reason we happen to have a second
        // joystick connected anyway, this setting will be fixed at the end of this function.
        qCDebug(JoystickManagerLog) << "And saving former primary as secondary: " << name;
        settings.setValue(_settingsKeyActiveJoystickSecondary, name);
    }
    // Save the settings so it persists reboots
    settings.setValue(_settingsKeyActiveJoystick, _activeJoystick->name());

    // Only proceed here if we have more than 1 joystick
    if (_name2JoystickMap.count() <= 1) {
        // But We could be here also if we disconnect first joystick, second joystick would be set as active by the block above, so we
        // would have a situation where primary and secondary are the same. If this is the case, set active joystick secondary to null before proceeding
        if (_activeJoystick != nullptr && _activeJoystick == _activeJoystickSecondary) {
            qCDebug(JoystickManagerLog) << "Secondary joystick moved to primary. Removing reference to secondary one so they don't point to the same joystick...";
            setActiveJoystickSecondary(nullptr);
        }
        return;
    }

    // Get the secondary joystick name in settings
    QString nameSecondary = settings.value(_settingsKeyActiveJoystickSecondary).toString();
    
    // If we don't have settings ( first boot, or never before a joystick was connected ), use second in the list
    if (nameSecondary.isEmpty()) {
        QList name2JoystickMapKeys = _name2JoystickMap.keys();
        qCDebug(JoystickManagerLog) << "Secondary joystick not set in settings, using second in the connected devices list: " << name2JoystickMapKeys.value(qint64(1));
        nameSecondary = name2JoystickMapKeys.value(qint64(1));
    } else {
        qCDebug(JoystickManagerLog) << "Secondary joystick settings available, trying to use: " << nameSecondary;
    }

    qCDebug(JoystickManagerLog) << "About to set Secondary Active joystick to: " << nameSecondary << ", is it presesent on our map? : " << _name2JoystickMap.contains(nameSecondary);
    qCDebug(JoystickManagerLog) << "Our map is: " << _name2JoystickMap;
    setActiveJoystickSecondary(_name2JoystickMap.value(nameSecondary));
    // Paranoid check, if for some reason code above is changed and we get something wrong,
    // _activeJoystickSecondary could be nullptr, and it would crash when accesing it
    if (!_activeJoystickSecondary) {
        qCDebug(JoystickManagerLog) << "active joystick secondary is null, not saving in settings";
        setActiveJoystickSecondary(nullptr);
        return;
    }
    qCDebug(JoystickManagerLog) << "active joystick secondary set up correctly, saving in settings: " << _activeJoystickSecondary->name();
    settings.setValue(_settingsKeyActiveJoystickSecondary, _activeJoystickSecondary->name());
}

Joystick* JoystickManager::activeJoystick(void)
{
    return _activeJoystick;
}

Joystick* JoystickManager::activeJoystickSecondary(void)
{
    return _activeJoystickSecondary;
}

void JoystickManager::setActiveJoystick(Joystick* joystick)
{
    QSettings settings;

    if (joystick != nullptr && !_name2JoystickMap.contains(joystick->name())) {
        qCWarning(JoystickManagerLog) << "Set active not in map" << joystick->name();
        return;
    }

    if (_activeJoystick == joystick) {
        return;
    }

    if (_activeJoystick) {
        // Disconnect this, as we are not longer interested on the status of the no longer active Joystick
        disconnect(_activeJoystick, &Joystick::calibratedChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
        disconnect(_activeJoystick, &Joystick::mainControlEnabledChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
        disconnect(_activeJoystick, &Joystick::gimbalEnabledChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);

        qCDebug(JoystickManagerLog) << "Stop polling active primary Joystick: " << _activeJoystick->name() << " Because of setActiveJoystick";
        _activeJoystick->stopPolling();
    }

    _activeJoystick = joystick;

    if (_activeJoystick != nullptr) {
        qCDebug(JoystickManagerLog) << "Setting active Joystick in settings:" << _activeJoystick->name();

        settings.beginGroup(_settingsGroup);
        settings.setValue(_settingsKeyActiveJoystick, _activeJoystick->name());

        // connect signals to evaluate automatically the multi joystick config state
        connect(_activeJoystick, &Joystick::calibratedChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
        connect(_activeJoystick, &Joystick::mainControlEnabledChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
        connect(_activeJoystick, &Joystick::gimbalEnabledChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
    }

    emit activeJoystickChanged(_activeJoystick);
    emit activeJoystickNameChanged(_activeJoystick?_activeJoystick->name():"");
    
    evaluateMultiJoystickConfigOk();
}

void JoystickManager::setActiveJoystickSecondary(Joystick* joystick)
{
    QSettings settings;

    if (joystick != nullptr && !_name2JoystickMap.contains(joystick->name())) {
        qCWarning(JoystickManagerLog) << "Set active secondary not in map" << joystick->name();
        return;
    }

    if (_activeJoystickSecondary == joystick) {
        return;
    }

    if (_activeJoystickSecondary) {
        // Disconnect this, as we are not longer interested on the status of the no longer active Joystick
        disconnect(_activeJoystickSecondary, &Joystick::calibratedChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
        disconnect(_activeJoystickSecondary, &Joystick::mainControlEnabledChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
        disconnect(_activeJoystickSecondary, &Joystick::gimbalEnabledChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);

        qCDebug(JoystickManagerLog) << "Stop polling active secondary Joystick: " << _activeJoystickSecondary->name() << " Because of setActiveJoystick";
        _activeJoystickSecondary->stopPolling();
    }

    _activeJoystickSecondary = joystick;

    if (_activeJoystickSecondary != nullptr) {
        qCDebug(JoystickManagerLog) << "Set active Secondary Joystick in settings:" << _activeJoystickSecondary->name();

        settings.beginGroup(_settingsGroup);
        settings.setValue(_settingsKeyActiveJoystickSecondary, _activeJoystickSecondary->name());

        // connect signals to evaluate automatically the multi joystick config state
        connect(_activeJoystickSecondary, &Joystick::calibratedChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
        connect(_activeJoystickSecondary, &Joystick::mainControlEnabledChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
        connect(_activeJoystickSecondary, &Joystick::gimbalEnabledChanged, this, &JoystickManager::evaluateMultiJoystickConfigOk);
    }

    emit activeJoystickSecondaryChanged(_activeJoystickSecondary);
    emit activeJoystickSecondaryNameChanged(_activeJoystickSecondary?_activeJoystickSecondary->name():"");
    
    evaluateMultiJoystickConfigOk();
}

QVariantList JoystickManager::joysticks(void)
{
    QVariantList list;

    for (const QString &name: _name2JoystickMap.keys()) {
        list += QVariant::fromValue(_name2JoystickMap[name]);
    }

    return list;
}

QStringList JoystickManager::joystickNames(void)
{
    return _name2JoystickMap.keys();
}

QString JoystickManager::activeJoystickName(void)
{
    return _activeJoystick ? _activeJoystick->name() : QString();
}

QString JoystickManager::activeJoystickSecondaryName(void)
{
    return _activeJoystickSecondary ? _activeJoystickSecondary->name() : QString();
}

void JoystickManager::evaluateMultiJoystickConfigOk(void)
{
    bool multiJoystickConfigOk = true;
    bool primaryNull           = (_activeJoystick == nullptr);
    bool secondaryNull         = (_activeJoystickSecondary == nullptr);
    _multiJoystickConfigMessage = "";
    
    if (!primaryNull) {
        if (!_activeJoystick->calibrated() && (_activeJoystick->mainControlEnabled() || _activeJoystick->gimbalEnabled()) ) {
            _multiJoystickConfigMessage = "Primary joystick is not calibrated and has main or gimbal control enabled";
            qCDebug(JoystickManagerLog) << "evaluateMultiJoystickConfigOk: " << _multiJoystickConfigMessage;
            multiJoystickConfigOk = false;
        }
    }

    // Only enter here if config is still good
    if (multiJoystickConfigOk && !secondaryNull) {
        if (!_activeJoystickSecondary->calibrated() && (_activeJoystickSecondary->mainControlEnabled() || _activeJoystickSecondary->gimbalEnabled()) ) {
            _multiJoystickConfigMessage = "Secondary joystick is not calibrated and has main or gimbal control enabled";
            qCDebug(JoystickManagerLog) << "evaluateMultiJoystickConfigOk: " << _multiJoystickConfigMessage;
            multiJoystickConfigOk = false;
        }
    }

    // Only enter here if config is still good
    if (multiJoystickConfigOk && !primaryNull && !secondaryNull) {
        // Check if primary and secondary are the same and not nullptr
        if (_activeJoystick == _activeJoystickSecondary) {
            _multiJoystickConfigMessage = "Primary and Secondary joysticks are the same";
            qCDebug(JoystickManagerLog) << "evaluateMultiJoystickConfigOk: " << _multiJoystickConfigMessage;
            multiJoystickConfigOk = false;
            // If they are the same, the call to this function will reorganize them, and move secondary to primary. Then it will call to evaluate it all again in joystick/secondaryChanged
            _setActiveJoysticksFromSettings();
        
        } else if (_activeJoystick->mainControlEnabled() && _activeJoystickSecondary->mainControlEnabled()) {
            _multiJoystickConfigMessage = "Both joysticks have main control enabled";
            qCDebug(JoystickManagerLog) << "evaluateMultiJoystickConfigOk: " << _multiJoystickConfigMessage;
            multiJoystickConfigOk = false;
        
        } else if (_activeJoystick->gimbalEnabled() && _activeJoystickSecondary->gimbalEnabled()) {
            _multiJoystickConfigMessage = "Both joysticks have gimbal control enabled";
            qCDebug(JoystickManagerLog) << "evaluateMultiJoystickConfigOk: " << _multiJoystickConfigMessage;
            multiJoystickConfigOk = false;
        }
    }

    _multiJoystickConfigOk = multiJoystickConfigOk;
    emit multiJoystickConfigOkChanged();
    emit multiJoystickConfigMessageChanged();
}

bool JoystickManager::setActiveJoystickName(const QString& name)
{
    if (_name2JoystickMap.contains(name)) {
        setActiveJoystick(_name2JoystickMap[name]);
        return true;
    } else {
        qCWarning(JoystickManagerLog) << "Set active not in map" << name;
        return false;
    }
}

bool JoystickManager::setActiveJoystickSecondaryName(const QString& name)
{
    if (_name2JoystickMap.contains(name)) {
        setActiveJoystickSecondary(_name2JoystickMap[name]);
        return true;
    } else {
        qCWarning(JoystickManagerLog) << "Set active secondary not in map" << name;
        return false;
    }
}

/*
 * TODO: move this to the right place: JoystickSDL.cc and JoystickAndroid.cc respectively and call through Joystick.cc
 */
void JoystickManager::_updateAvailableJoysticks()
{
#ifdef __sdljoystick__
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            qCDebug(JoystickManagerLog) << "SDL ERROR:" << SDL_GetError();
            break;
        case SDL_JOYDEVICEADDED:
            qCDebug(JoystickManagerLog) << "Joystick added:" << event.jdevice.which;
            _setActiveJoysticksFromSettings();
            break;
        case SDL_JOYDEVICEREMOVED:
            qCDebug(JoystickManagerLog) << "Joystick removed:" << event.jdevice.which;
            _setActiveJoysticksFromSettings();
            break;
        default:
            break;
        }
    }
#elif defined(__android__)
    _joystickCheckTimerCounter--;
    _setActiveJoysticksFromSettings();
    if (_joystickCheckTimerCounter <= 0) {
        _joystickCheckTimer.stop();
    }
#endif
}

void JoystickManager::restartJoystickCheckTimer()
{
    _joystickCheckTimerCounter = 5;
    _joystickCheckTimer.start(1000);
}

void JoystickManager::resetJoystickPrimarySecondarySettings()
{
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    settings.setValue(_settingsKeyActiveJoystick, "");
    settings.setValue(_settingsKeyActiveJoystickSecondary, "");
    _setActiveJoysticksFromSettings();
}