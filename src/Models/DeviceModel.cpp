/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include <QJsonDocument>
#include "DeviceModel.h"
#include "DevicePropertyModel.h"
#include <QJsonArray>

DeviceModel::DeviceModel(QObject *parent) : QQmlPropertyMap(this, parent),
    _communicationHandler(new ResourceCommunicationHandler("device", this))
{
    connect(_communicationHandler, &ResourceCommunicationHandler::newMessage, this, &DeviceModel::messageReceived);
}


bool DeviceModel::triggerFunction(QString name, QVariantMap parameters)
{
    QVariantMap msg;
    msg["command"] = "device:call";
    QVariantMap msgParameters;
    msgParameters["funcparams"] = parameters;
    msgParameters["funcname"] = name;
    msg["parameters"] = msgParameters;
    _communicationHandler->sendMessage(msg);
    return true;
}

DevicePropertyModel *DeviceModel::getProperty(QString name)
{
    DevicePropertyModel* prop = _properties.value(name, nullptr);
    if(prop != nullptr)
    {
        return prop;
    }
    else
    {
        prop = createPropertyModel(name);
        _properties.insert(name, prop);
        Q_EMIT propertiesChanged();
    }
    return prop;
}

bool DeviceModel::hasProperty(QString name)
{
    return _properties.contains(name);
}

void DeviceModel::setJSProperty(QString name, QJSValue val)
{
    sendVariant(name, val.toVariant());
}

QString DeviceModel::resource() const
{
    return _resource;
}

void DeviceModel::setResource(const QString &resource)
{
    if(_resource == resource)
        return;

    _initialized = false;
    Q_EMIT initializedChanged();
    _resource = resource;
    _communicationHandler->setDescriptor(_resource);
    _communicationHandler->attachModel();
    Q_EMIT resourceChanged();
}

QList<QObject*> DeviceModel::properties() const
{
    QObjectList objects;
    QListIterator<DevicePropertyModel*> it(_properties.values());
    while(it.hasNext())
    {
        objects << it.next();
    }
    return objects;
}

QMap<QString, DevicePropertyModel*> DeviceModel::deviceProperties() const
{
    return _properties;
}

bool DeviceModel::getConnected() const
{
    return _communicationHandler->isAttached();
}

ResourceCommunicationHandler::ModelState DeviceModel::getModelState() const
{
    return _communicationHandler->getState();
}

bool DeviceModel::deviceOnline() const
{
    return _online;
}

QVariantList DeviceModel::functions()
{
    return _functions;
}

QString DeviceModel::description() const
{
    return _description;
}

void DeviceModel::setDescription(const QString &description)
{
    QVariantMap msg;
    msg["command"] = "device:description";
    QVariantMap msgParameters;
    msgParameters["desc"] = description;
    msg["parameters"] = msgParameters;
    _communicationHandler->sendMessage(msg);
}

QString DeviceModel::uuid() const
{
    return _uuid;
}

QString DeviceModel::getType() const
{
    return _type;
}

void DeviceModel::setType(const QString &type)
{
    _type = type;
}

bool DeviceModel::getAvailable() const
{
    return !_temporary;
}

QString DeviceModel::getShortID() const
{
    return _suid;
}

bool DeviceModel::getInitialized() const
{
    return _initialized;
}

void DeviceModel::checkIfPropertiesAreEditable()
{
    QListIterator<QVariant> it(_functions);
    while(it.hasNext())
    {
        QString funcName = it.next().toMap()["name"].toString();
        if(funcName.startsWith("set"))
        {
            funcName = funcName.remove(0,3).toLower();
            if(_properties.contains(funcName))
            {

                _properties[funcName]->setEditable(true);
            }
        }
    }
}

DevicePropertyModel* DeviceModel::createPropertyModel(QString name, QVariantMap metadata)
{
    auto prop = new DevicePropertyModel(name, this, metadata);
    connect(prop, &DevicePropertyModel::sendValueToDevice, this, &DeviceModel::sendVariant);
    connect(prop, &DevicePropertyModel::metadataEdited, this, &DeviceModel::metadataEdited);

    return prop;
}


void DeviceModel::sendVariant(QString property, QVariant value)
{
    QVariantMap msg;
    msg["command"] = "device:setproperty";
    QVariantMap msgParameters;
    msgParameters["property"] = property;
    msgParameters["value"] = value;
    msg["parameters"] = msgParameters;
    _communicationHandler->sendMessage(msg);
}

void DeviceModel::metadataEdited(QString name, QString key, QVariant value)
{
    QVariantMap msg;
    msg["command"] = "device:meta:set";
    QVariantMap parameters;
    QVariantMap data;
    data[key] = value;
    parameters[name] = data;
    msg["parameters"] = parameters;
    _communicationHandler->sendMessage(msg);
}

QVariant DeviceModel::updateValue(const QString &key, const QVariant &input)
{
    sendVariant(key, input);

    // returning the initial (uncommited)
    // value would be more correct, but this results in ugly flickering

    return input; // this->value(key);
}

void DeviceModel::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString command = msg["command"].toString();
    QString error = msg["errorstring"].toString();


    if(!error.isEmpty())
        qWarning()<<Q_FUNC_INFO<<": "<<error;

    QVariantMap parameters = msg["parameters"].toMap();

    if(command =="device:dump")
    {
        _functions = parameters["funcs"].toList();
        Q_EMIT functionsChanged();

        QVariantList properties = parameters["props"].toList();
        QListIterator<QVariant> it(properties);
        while(it.hasNext())
        {
            QVariantMap property = it.next().toMap();
            QString propertyName = property["name"].toString();
            DevicePropertyModel* model = _properties.value(propertyName, nullptr);
            if(model != nullptr)
            {
                model->init(property);
            }
            else
            {
                model = createPropertyModel(propertyName, property);
            }
            _properties.insert(propertyName, model);
            this->insert(property["name"].toString(), model->getRealValue());
        }

        Q_EMIT propertiesChanged();
        _online = parameters["on"].toBool();
        Q_EMIT deviceOnlineChanged();
        _description = parameters["desc"].toString();
        Q_EMIT descriptionChanged();
        _uuid =  parameters["uuid"].toString();
        Q_EMIT uuidChanged();
        _type =  parameters["type"].toString();
        Q_EMIT typeChanged();
        _temporary =  parameters["tmp"].toBool();
        Q_EMIT tempChanged();
        _suid =  parameters["suid"].toString();
        Q_EMIT shortIDChanged();
        checkIfPropertiesAreEditable();
        _initialized = true;
        Q_EMIT initializedChanged();
        return;
    }

    if(command =="device:data")
    {
        QString subject = parameters["subj"].toString();
        QVariantMap data = parameters["data"].toMap();
        Q_EMIT dataReceived(subject, data);
        return;
     }

    if(command =="device:property:set")
    {
        QString property = parameters["property"].toString();
        QVariant value = parameters["value"];
        this->insert(property, value);
        return;
    }

    if(command =="device:statuschanged")
    {
        _online = parameters["online"].toBool();
        Q_EMIT deviceOnlineChanged();
        return;
    }

    if(command =="device:tmpchanged")
    {
        _temporary = parameters["tmp"].toBool();
        Q_EMIT tempChanged();
        return;
    }

    if(command =="device:meta:set")
    {
        QString property = parameters.firstKey();
        QVariantMap data = parameters.first().toMap();
        QString key = data.firstKey();
        QString value = data.first().toString();

        DevicePropertyModel* prop = _properties.value(property, nullptr);
        if(prop != nullptr)
        {
            prop->setMedatada(key, value);
        }
        return;
    }

    if(command == "device:prop:set")
    {
        QString property = parameters.firstKey();
        QVariantMap data = parameters.first().toMap();
        QMapIterator<QString, QVariant> it(data);

        DevicePropertyModel* model = _properties.value(property, nullptr);
        if(model == nullptr)
            return;

        while(it.hasNext())
        {
            it.next();
            QString key = it.key();
            QVariant val = it.value();

            if(key == "real")
            {
                model->setRealValue(val);
                this->insert(property, val);
            }

            if(key == "set")
            {
                model->setSetValue(val);
            }

            if(key == "dirty")
            {
                model->setDirty(val.toBool());
            }

            if(key == "timestamp")
            {
                model->setTimestamp(val.toLongLong());
            }
        }

        model->emitValueChanged();
    }

    if(command == "device:description")
    {
        QString desc = parameters["desc"].toString();
        _description = desc;
        Q_EMIT descriptionChanged();
    }
}

void DeviceModel::connectObject()
{
    _communicationHandler->attachModel();
}

void DeviceModel::disconnectObject()
{
    _communicationHandler->detachModel();
}
