#pragma once
#include "../Models/Device.h"

class QQmlEngine;
class StandaloneDevice : public Device
{
    Q_OBJECT

public:
    static QObject* instanceAsQObject(QQmlEngine *engine = nullptr, QJSEngine *scriptEngine = nullptr);
    static StandaloneDevice* instance();
    Q_INVOKABLE void setPermission(QString permission, bool granted);

protected:
    virtual void initDevice(QVariantMap parameters = QVariantMap()) override;
    QVariantMap _permissions;

private:
    StandaloneDevice(QObject* parent = nullptr);
    static StandaloneDevice* _instance;

protected slots:
    virtual void messageReceived(QVariant message) override;
};

