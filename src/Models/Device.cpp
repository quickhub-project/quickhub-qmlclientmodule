/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "Device.h"
#include "../Core/ConnectionManager.h"
#include <QJsonParseError>
#include <QQmlEngine>
#include <QDebug>
#include <QQmlContext>

Device::Device(QObject *parent) : Device(new VirtualConnection(ConnectionManager::instance()->getConnection()), this)
{
}

Device::Device(VirtualConnection *vconnection, QObject *parent) : QQmlPropertyMap(this, parent)
{
    _conn = vconnection;
    connect(_conn, &VirtualConnection::connected, this, &Device::connectedSlot);
    connect(_conn, &VirtualConnection::disconnected, this, &Device::disconnectedSlot);
    connect(_conn, &VirtualConnection::messageReceived, this, &Device::messageReceived);
    if(_conn->getConnectionState() == VirtualConnection::CONNECTED)
    {
        initDevice();
    }

}

Device::~Device()
{
    _settings.sync();
}

void Device::registerFunction(QString name, QJSValue callback)
{
    if(callback.isCallable())
    {
        _functions.insert(name, callback);
    }
}


void Device::registerPropertyWithInitValue(QString name,  QJSValue callback, QVariant init)
{
    QChar firstChar = name.at(0).toUpper();
    QString upper = name;
    QString setterName = "set" + upper.replace(0,1,firstChar);

    if(callback.isCallable())
    {
        _functions.insert(setterName, callback);
        QVariant loadedVal= _settings.value("Devices/"+uuid()+"/"+name);

        if(!loadedVal.isValid() && init.isValid())
        {
            loadedVal = init;
        }

        QVariantMap data;
        data["val"] = loadedVal;
        QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
        QJSValue argJS = engine->toScriptValue<QVariantMap>(data);
        callback.call({argJS});
        setProperty(name, loadedVal);
    }
}


void Device::registerProperty(QString name, QJSValue callback, QVariant value)
{
    QChar firstChar = name.at(0).toUpper();
    QString upper = name;
    QString setterName = "set" + upper.replace(0,1,firstChar);

    if(callback.isCallable())
    {
        _functions.insert(setterName, callback);
        if(value.isValid())
        {
            setProperty(name, value);
        }
        else
        {
            QVariant loadedVal= _settings.value("Devices/"+uuid()+"/"+name);
            QVariantMap data;
            data["val"] = loadedVal;
            QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
            QJSValue argJS = engine->toScriptValue<QVariantMap>(data);
            callback.call({argJS});
            setProperty(name, loadedVal);
        }
    }
}

void Device::setProperty(QString name, QVariant value)
{
    insert(name, value);
    propertyDataChanged(name, value);
}

void Device::sendMessage(QString subject, QVariantMap data)
{
     QVariantMap msg;
     msg["cmd"] = "msg";
     QVariantMap parameters;
     parameters["subject"] = subject;
     if(!data.isEmpty())
            parameters["data"] = data;

     msg["params"] = parameters;
     _conn->sendVariant(msg);
}

void Device::start()
{
    if(_conn->getConnectionState() == VirtualConnection::CONNECTED)
    {
        initDevice();
    }
    else
    {
        _readyForInit = true;
    }
    _settings.sync();
}

QString Device::uuid() const
{
    return _uuid;
}

void Device::setUuid(const QString &uuid)
{
    _uuid = uuid;
    Q_EMIT uuidChanged();
}

QVariant Device::updateValue(const QString &key, const QVariant &input)
{
    propertyDataChanged(key, input);
    return input;
}

QString Device::shortID() const
{
    return _shortID;
}

void Device::setShortID(const QString &shortID)
{
    _shortID = shortID;
    Q_EMIT shortIDChanged();
}

bool Device::initialized() const
{
    return _initialized;
}

bool Device::connected() const
{
    return _connected;
}

void Device::propertyDataChanged(const QString &key, const QVariant &input)
{
    _settings.setValue("Devices/"+uuid()+"/"+key, input);
   Q_EMIT propertyUpdate(key, input);

   if(!_connected)
   {
       _tempCache.insert(key, input);
       return;
   }

    QVariantMap msg;
    msg["cmd"] = "set";
    QVariantMap parameters;
    parameters[key] = input;
    msg["params"] = parameters;
    _conn->sendVariant(msg);
}

QString Device::type() const
{
    return _type;
}

void Device::setType(const QString &type)
{
    _type = type;
}

void Device::connectToServer()
{
}

void Device::connectedSlot()
{
    if(_readyForInit && !_initialized)
    {
        initDevice();
    }
}

void Device::disconnectedSlot()
{
    _initialized = false;
}

void Device::initDevice(QVariantMap parameters)
{
    QVariantMap msg;
    msg["command"] = "node:register";
    parameters["id"] = _uuid;
    parameters["sid"] = _shortID;
    QListIterator<QString> propertyKeysIt(this->keys());
    QVariantMap properties;
    while(propertyKeysIt.hasNext())
    {
       const QString& key = propertyKeysIt.next();
       properties.insert(key, this->value(key));
    }

    parameters["properties"] = properties;


    QVariantList functions;
    QListIterator<QString> funcKeysIt(_functions.keys());
    while(funcKeysIt.hasNext())
    {
        const QString& key = funcKeysIt.next();
        QVariantMap map;
        map["name"] = key;
        functions << map;
    }
    parameters["functions"] = functions;


    parameters["type"] = _type;
    parameters["key"] = _settings.value("Devices/"+uuid()+"_key", 0).toUInt();
    msg["parameters"] = parameters;

    _conn->sendVariant(msg);
    _initialized = true;
    Q_EMIT initializedChanged();
}

void Device::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString command = msg["cmd"].toString();

    if(command == "call")
    {
        QVariantMap parameters = msg["params"].toMap();
        QString function = parameters.firstKey();
        if(_functions.contains(function))
        {
            if(parameters.isEmpty())
            {
                _functions[function].call();
            }
            else
            {
                QVariantMap argVariant = parameters.value(parameters.firstKey()).toMap();
                QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
                if(!engine)
                {
                    qWarning()<<Q_FUNC_INFO << "No  JS-Engine!";
                    return;
                }
                QJSValue argJS = engine->toScriptValue<QVariantMap>(argVariant);
                _functions[function].call({argJS});
            }
        }
        return;
    }

   if(command == "init")
    {
      QVariantList parameters = msg["params"].toList();
      QListIterator<QVariant> listIt = parameters;
      QQmlEngine* engine = QQmlEngine::contextForObject(this)->engine();
      while(listIt.hasNext())
      {
          QVariantMap item = listIt.next().toMap();
          QJSValue argJS = engine->toScriptValue<QVariantMap>(item["args"].toMap());
          QString function = item["func"].toString();
          _functions[function].call({argJS});
      }
      _connected = true;
      Q_EMIT connectedChanged();

      if(!_tempCache.isEmpty())
      {
          QVariantMap msg;
          msg["cmd"] = "set";
          msg["params"] = _tempCache;
          _conn->sendVariant(msg);
          _tempCache.clear();
      }
    }

   if(command == "setkey")
   {
       quint32 key = msg["params"].toUInt();
       _settings.setValue("Devices/"+uuid()+"_key", key);
   }
}

