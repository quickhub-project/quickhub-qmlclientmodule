/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "DeviceLogic.h"
#include "../Core/CloudModel.h"

DeviceLogic::DeviceLogic(QObject *parent) : QObject(parent),
    _conn(new VirtualConnection(CloudModel::instance()->getConnection()))
{
    connect(_conn, &VirtualConnection::connected, this, &DeviceLogic::connectedSlot);
    connect(_conn, &VirtualConnection::disconnected, this, &DeviceLogic::disconnectedSlot);
    connect(_conn, &VirtualConnection::messageReceived, this, &DeviceLogic::messageReceived);
    if(_conn->getConnectionState() == VirtualConnection::CONNECTED)
    {
        initDevice();
    }
}

QString DeviceLogic::shortID() const
{
    return _shortID;
}

void DeviceLogic::setShortID(const QString &shortID)
{
    _shortID = shortID;
    Q_EMIT shortIDChanged();
}


void DeviceLogic::registerProperty(devicePropertyPtr property)
{
    _properties.insert(getSetterName(property->getName()), property);
    connect(property.data(), &DeviceLogicProperty::valueChanged, this, &DeviceLogic::setProperty);
    QVariant loadedVal = _settings.value("Devices/"+uuid()+"/"+property->getName());
    property->initialize(loadedVal);
}

void DeviceLogic::setProperty(QString name, QVariant value)
{
    devicePropertyPtr prop = _properties.value(getSetterName(name));
    if(!prop.isNull())
    {
        _settings.setValue("Devices/"+uuid()+"/"+name, value);
        _settings.sync();
       Q_EMIT propertyUpdate(name, value);

       if(!_connected)
       {
           _tempCache.insert(name, value);
           return;
       }

        QVariantMap msg;
        msg["cmd"] = "set";
        QVariantMap parameters;
        parameters[name] = value;
        msg["params"] = parameters;
        _conn->sendVariant(msg);
    }
}

void DeviceLogic::sendMessage(QString subject, QVariantMap data)
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

QString DeviceLogic::uuid() const
{
    return _uuid;
}

void DeviceLogic::setUuid(const QString &uuid)
{
    _uuid = uuid;
    Q_EMIT uuidChanged();
}

void DeviceLogic::initDevice()
{
    QVariantMap msg;
    msg["command"] = "node:register";
    QVariantMap parameters;
    parameters["id"] = _uuid;
    parameters["sid"] = _shortID;
    QMapIterator<QString, devicePropertyPtr> propertyKeysIt(_properties);
    QVariantMap properties;
    QVariantList functions;

    while(propertyKeysIt.hasNext())
    {
       propertyKeysIt.next();
       devicePropertyPtr prop = propertyKeysIt.value();
       if(prop->getType() != DeviceLogicProperty::READ_ONLY)
            _functions << propertyKeysIt.key();

       const QString& key = prop->getName();
       QVariant value = prop->getValue();
       properties.insert(key, value);
    }

    parameters["properties"] = properties;

    QListIterator<QString> funcKeysIt(_functions);
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

QString DeviceLogic::getSetterName(QString propertyName)
{
    QChar firstChar = propertyName.at(0).toUpper();
    QString upper = propertyName;
    return "set" + upper.replace(0,1,firstChar);
}

bool DeviceLogic::getConnected() const
{
    return _connected;
}

bool DeviceLogic::getInitialized() const
{
    return _initialized;
}

QString DeviceLogic::getType() const
{
    return _type;
}

void DeviceLogic::setType(const QString &type)
{
    _type = type;
}


void DeviceLogic::start()
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

void DeviceLogic::connectToServer()
{

}

void DeviceLogic::connectedSlot()
{
    if(_readyForInit && !_initialized)
    {
        initDevice();
    }
}

void DeviceLogic::disconnectedSlot()
{
    _initialized = false;
}

void DeviceLogic::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString command = msg["cmd"].toString();

    if(command == "call")
    {
        QVariantMap parameters = msg["params"].toMap();
        QString function = parameters.firstKey();
        devicePropertyPtr prop = _properties.value(function);
        QVariantMap args;
        if(!parameters.isEmpty())
            args = parameters.value(parameters.firstKey()).toMap();

        if(!prop.isNull())
        {
            prop->requestValueChange(args.value("val"));
        }
        else if(_functions.contains(function))
        {
            Q_EMIT functionCalled(function, args);
        }
        return;
    }

   if(command == "init")
    {
      QVariantList parameters = msg["params"].toList();
      QListIterator<QVariant> listIt = parameters;
      while(listIt.hasNext())
      {
          QVariantMap item = listIt.next().toMap();
          QVariantMap args = item["args"].toMap();
          QString function = item["func"].toString();
          devicePropertyPtr prop = _properties.value(function);
          if(!prop.isNull())
          {
                prop->requestValueChange(args.value("val"));
          }
          else if(_functions.contains(function))
            Q_EMIT functionCalled(function, args);
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
