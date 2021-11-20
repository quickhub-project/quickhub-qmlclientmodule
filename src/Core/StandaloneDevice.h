#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include <QObject>
#include "../Models/Device.h"

class QQmlEngine;
class StandaloneDevice : public Device
{
    Q_OBJECT

public:
    Q_INVOKABLE void requestPermission(QString permission, bool granted);
    static QObject* instanceAsQObject(QQmlEngine *engine = nullptr, QJSEngine *scriptEngine = nullptr);
    static StandaloneDevice* instance();

protected:
    virtual void initDevice(QVariantMap parameters = QVariantMap()) override;
    QVariantMap _permissions;

private:
    static StandaloneDevice* _instance;
    StandaloneDevice(QObject* parent = nullptr);

protected slots:
    virtual void messageReceived(QVariant message) override;

};

#endif // BASEDEVICE_H
