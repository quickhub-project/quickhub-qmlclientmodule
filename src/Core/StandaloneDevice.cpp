#include "StandaloneDevice.h"
#include "ConnectionManager.h"
#include <QApplication>
#include <QMetaObject>
#include <QDebug>

StandaloneDevice* StandaloneDevice::_instance = nullptr;

StandaloneDevice::StandaloneDevice(QObject *parent) : Device(this, ConnectionManager::instance()->getVConnection(), parent)
{
#if QT_VERSION > QT_VERSION_CHECK(5, 11, 0)
    this->setUuid(QSysInfo::machineUniqueId());
#endif

}

void StandaloneDevice::initDevice(QVariantMap parameters)
{
    Q_UNUSED(parameters)
    ConnectionManager::instance()->setConnectionState(ConnectionManager::STATE_Authenticating);
    QVariantMap params;
    params["permissions"] = _permissions;
    Device::initDevice(params);
}

void StandaloneDevice::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString command = msg["cmd"].toString();

    if(command == "settoken")
    {
       ConnectionManager::instance()->setToken( msg["params"].toString());
       ConnectionManager::instance()->setConnectionState(ConnectionManager::STATE_Authenticated);
       return;
    }

    Device::messageReceived(message);
}

void StandaloneDevice::setPermission(QString permission, bool granted)
{
    _permissions.insert(permission, granted);
}

QObject *StandaloneDevice::instanceAsQObject(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return instance();
}

StandaloneDevice *StandaloneDevice::instance()
{
    if(_instance == nullptr)
        _instance = new StandaloneDevice();
    return _instance;
}

