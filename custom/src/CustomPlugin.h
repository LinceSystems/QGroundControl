#pragma once

#include "QGCCorePlugin.h"
#include "QGCOptions.h"
#include "QGCLoggingCategory.h"

#include <QTranslator>

class CustomOptions;
class CustomPlugin;

Q_DECLARE_LOGGING_CATEGORY(CustomPluginLog)

class CustomFlyViewOptions : public QGCFlyViewOptions
{
public:
    CustomFlyViewOptions(CustomOptions* options, QObject* parent = nullptr);
};

class CustomOptions : public QGCOptions
{
public:
    CustomOptions(CustomPlugin*, QObject* parent = nullptr);

    // Overrides from QGCOptions
    QGCFlyViewOptions*      flyViewOptions(void) final;

private:
    CustomFlyViewOptions* _flyViewOptions = nullptr;
};

class CustomPlugin : public QGCCorePlugin
{
    Q_OBJECT
public:
    CustomPlugin(QGCApplication* app, QGCToolbox *toolbox);
    ~CustomPlugin();

    // Overrides from QGCCorePlugin
    QGCOptions*         options             (void) final;

    // Overrides from QGCTool
    void                setToolbox          (QGCToolbox* toolbox) override;

private:
    CustomOptions*  _options = nullptr;
};
