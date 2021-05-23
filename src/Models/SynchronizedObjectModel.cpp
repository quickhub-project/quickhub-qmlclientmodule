/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "SynchronizedObjectModel.h"
#include <QDebug>
#include "../Core/CloudModel.h"
#include <QJsonDocument>
SynchronizedObjectModel::SynchronizedObjectModel(QObject *parent) : QQmlPropertyMap(this, parent),
    _communicationHandler(new ResourceCommunicationHandler("object", this))
{
    connect(_communicationHandler,SIGNAL(newMessage(QVariant)), this, SLOT(messageReceived(QVariant)));
    connect(_communicationHandler,SIGNAL(attachedChanged()), this, SIGNAL(connectedChanged()));
    connect(_communicationHandler,SIGNAL(stateChanged()), this, SLOT(modelStateChangedSlot()));
}

SynchronizedObjectModel::~SynchronizedObjectModel()
{
}

void SynchronizedObjectModel::connectObject()
{
    _communicationHandler->attachModel();
}

void SynchronizedObjectModel::disconnectObject()
{
    _communicationHandler->detachModel();
}

QVariant SynchronizedObjectModel::updateValue(const QString &key, const QVariant &input)
{
    QVariantMap msg;
    msg["command"] = "object:property:set";
    QVariantMap parameters;
    parameters["property"] = key;
    parameters["data"] = input;
    msg["parameters"]  = parameters;
    _communicationHandler->sendMessage(msg);
    return input;
}


bool SynchronizedObjectModel::getConnected() const
{
    return _communicationHandler->isAttached();
}

QVariantMap SynchronizedObjectModel::getMetadata() const
{
    return _metadata;
}

bool SynchronizedObjectModel::initialized() const
{
    return _initialized;
}

void SynchronizedObjectModel::setProperty(QString key, QVariant value)
{
    QVariantMap msg;
    msg["command"] = "object:property:set";
    QVariantMap parameters;
    parameters["property"] = key;
    parameters["data"] = value;
    msg["parameters"]  = parameters;
    _communicationHandler->sendMessage(msg);
    this->insert(key, value);
    _keys << key;
    Q_EMIT keysChanged();
}

ResourceCommunicationHandler::ModelState SynchronizedObjectModel::getModelState() const
{
    return _communicationHandler->getState();
}

QVariantMap SynchronizedObjectModel::getFilter() const
{
    return _filter;
}

void SynchronizedObjectModel::setFilter(const QVariantMap &filter)
{
    QVariantMap msg;
    msg[QStringLiteral("command")] = "object:filter";
    QVariantMap parameters;
    parameters[QStringLiteral("data")] = filter;
    msg[QStringLiteral("parameters")] = parameters;
    _communicationHandler->sendMessage(msg);
    Q_EMIT filterChanged();
}

QString SynchronizedObjectModel::resource() const
{
    return _resource;
}

void SynchronizedObjectModel::setResource(const QString &resourceName)
{
    if(_resource == resourceName)
        return;

    _initialized = false;
    Q_EMIT initializedChanged();
    _resource = resourceName;
    _communicationHandler->setDescriptor(_resource);
    _communicationHandler->attachModel();
    Q_EMIT resourceChanged();
}

QStringList SynchronizedObjectModel::keys()
{
    return _keys.values();
}


void SynchronizedObjectModel::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString cmd = msg["command"].toString();

    QVariantMap parameters = msg["parameters"].toMap();
    QVariant data = parameters["data"];

    if(cmd == "object:dump")
    {
        QVariantMap parameters = msg["parameters"].toMap();
        QVariantMap map = parameters["data"].toMap();
        _metadata = parameters["metadata"].toMap();
        QMapIterator<QString, QVariant> it(map);

        while(it.hasNext())
        {
            it.next();
            QString key = it.key();
            QVariant value = it.value().toMap()["data"];
            this->insert(key, value);
            _keys << key;
            _objectdata.insert(key, it.value());
        }

        _initialized = true;
        Q_EMIT metadataChanged();
        Q_EMIT initializedChanged();
        Q_EMIT keysChanged();
        return;
    }

    if(cmd == "object:property:set")
    {
        QString key = parameters["property"].toString();
        QVariant value = parameters["data"];
        _keys << key;
        this->insert(key, value);
        Q_EMIT keysChanged();
    }
}

void SynchronizedObjectModel::modelStateChangedSlot()
{
    if(_communicationHandler->getState() == BaseCommunicationHandler::MODEL_DISCONNECTED)
    {
        QStringList keys = _keys.values();
        _keys.clear();
        Q_EMIT keysChanged();
        QListIterator<QString>it(keys);
        while(it.hasNext())
        {
            this->clear(it.next());
        }
    }
    Q_EMIT modelStateChanged();
}
