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
    QString name = settings.value(_settingsKeyActiveJoystick).toString();

    if (name.isEmpty()) {
        name = _name2JoystickMap.first()->name();
    }
    // Set by default active joystick to first joystick, if not found in settings
    setActiveJoystick(_name2JoystickMap.value(name, _name2JoystickMap.first()));
    settings.setValue(_settingsKeyActiveJoystick, _activeJoystick->name());

    // Only proceed here if we have more than 1 joystick
    if (_name2JoystickMap.count() <= 1) {
        return;
    }

    QString nameSecondary = settings.value(_settingsKeyActiveJoystickSecondary).toString();
    QList name2JoystickMapKeys = _name2JoystickMap.keys();
    
    // By default joystick secondary is second joystick, if not found in settings
    if (nameSecondary.isEmpty()) {
        // Assign to second value
        nameSecondary = name2JoystickMapKeys.value(qint64(1));
    }

    setActiveJoystickSecondary(_name2JoystickMap.value(nameSecondary));
    // Paranoid check, if for some reason code above is changed and we get something wrong,
    // _activeJoystickSecondary could be nullptr, and it would crash when accesing it
    if (!_activeJoystickSecondary) {
        qCDebug(JoystickManagerLog) << "active joystick secondary is null";
        setActiveJoystickSecondary(nullptr);
        return;
    }
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
        qCDebug(JoystickManagerLog) << "Stop polling active primary Joystick: " << _activeJoystick->name() << " Because of setActiveJoystick";
        _activeJoystick->stopPolling();
    }

    _activeJoystick = joystick;

    if (_activeJoystick != nullptr) {
        qCDebug(JoystickManagerLog) << "Set active:" << _activeJoystick->name();

        settings.beginGroup(_settingsGroup);
        settings.setValue(_settingsKeyActiveJoystick, _activeJoystick->name());
    }

    emit activeJoystickChanged(_activeJoystick);
    emit activeJoystickNameChanged(_activeJoystick?_activeJoystick->name():"");
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
        qCDebug(JoystickManagerLog) << "Stop polling active secondary Joystick: " << _activeJoystickSecondary->name() << " Because of setActiveJoystick";
        _activeJoystickSecondary->stopPolling();
    }

    _activeJoystickSecondary = joystick;

    if (_activeJoystickSecondary != nullptr) {
        qCDebug(JoystickManagerLog) << "Set active secondary:" << _activeJoystickSecondary->name();

        settings.beginGroup(_settingsGroup);
        settings.setValue(_settingsKeyActiveJoystickSecondary, _activeJoystickSecondary->name());
    }

    emit activeJoystickSecondaryChanged(_activeJoystickSecondary);
    emit activeJoystickSecondaryNameChanged(_activeJoystickSecondary?_activeJoystickSecondary->name():"");
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
