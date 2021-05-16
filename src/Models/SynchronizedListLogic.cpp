/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include <QMetaProperty>
#include <QDebug>
#include <QJsonDocument>

#include "SynchronizedListLogic.h"
#include "../Core/CloudModel.h"


SynchronizedListLogic::SynchronizedListLogic(QObject *parent) : QObject(parent),
    _communicationHandler(new ResourceCommunicationHandler("synclist", this))
{
    connect(_communicationHandler,SIGNAL(newMessage(QVariant)), this, SLOT(messageReceived(QVariant)));
    connect(_communicationHandler,SIGNAL(attachedChanged()), this, SIGNAL(connectedChanged()));
    connect(_communicationHandler,SIGNAL(stateChanged()), this, SLOT(stateChanged()));
}

QString SynchronizedListLogic::getUserIDForIndex(int index)
{
    if(_metaInfo.size() > index && index >= 0)
    {
        return _metaInfo.at(index).user;
    }
    return "unknown";
}

qint64 SynchronizedListLogic::getTimestampForIndex(int index)
{
    if(_metaInfo.size() > index && index >= 0)
    {
        return _metaInfo.at(index).lastUpdate;
    }
    return -1;
}

QString SynchronizedListLogic::getUUIDForIndex(int index)
{
    if(_metaInfo.size() > index && index >= 0)
    {
        return _metaInfo.at(index).uuid;
    }
    return "";
}

void SynchronizedListLogic::insertAt(int index, QVariantMap obj)
{
    QVariantMap parameters;
    parameters[QStringLiteral("index")] = index;
    parameters[QStringLiteral("data")] = obj;

    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:insertat";
    msg[QStringLiteral("parameters")] = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListLogic::append(QObject *obj)
{
    if(!obj)
        return;

    QVariantMap data;
    for(int i = 0; i < obj->metaObject()->propertyCount(); i++)
    {
        const char* key = obj->metaObject()->property(i).name();
        data[key] = obj->property(key);
    }

    append(data);
}

void SynchronizedListLogic::append(QVariantMap data)
{
    QVariantMap parameters;
    parameters[QStringLiteral("data")] = data;

    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:append";
    msg[QStringLiteral("parameters")] = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListLogic::appendList(QVariantList list)
{
    QVariantMap parameters;
    parameters[QStringLiteral("data")] = list;
    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:appendlist";
    msg[QStringLiteral("parameters")] = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListLogic::set(int index, QVariantMap data)
{
    QVariantMap parameters;
    parameters[QStringLiteral("data")] = data;
    parameters[QStringLiteral("index")] = index;
    if(index >= 0 && index < _metaInfo.count())
    {
        parameters[QStringLiteral("uuid")] = _metaInfo.at(index).uuid;
    }

    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:set";
    msg[QStringLiteral("parameters")] = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListLogic::clear()
{
    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:clear";
    _communicationHandler->sendMessage(msg);
}

int SynchronizedListLogic::getIndexForUUID(QString uuid)
{
    return checkAndCorrectIndex(-1, uuid);
}

void SynchronizedListLogic::remove(int index)
{
    if(index >= 0 && index < _metaInfo.count())
    {
        QVariantMap msg;
        msg[QStringLiteral("command")] = "synclist:remove";
        QVariantMap parameters;
        parameters[QStringLiteral("uuid")] = _metaInfo.at(index).uuid;
        parameters[QStringLiteral("index")] = index;
        msg[QStringLiteral("parameters")] = parameters;
        _communicationHandler->sendMessage(msg);
    }
}

void SynchronizedListLogic::deleteList()
{
    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:delete";
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListLogic::setFilter(const QVariantMap &filter)
{
    QVariantMap msg;
    QString base = _resource.split(":").first();
    _communicationHandler->setDescriptor(base+ ":" + QJsonDocument::fromVariant(filter).toJson(QJsonDocument::Compact));
    msg[QStringLiteral("command")] = "synclist:filter";
    QVariantMap parameters;
    parameters[QStringLiteral("data")] = filter;
    msg[QStringLiteral("parameters")] = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListLogic::setProperty(int index, QString property, QVariant val)
{
    if((index < 0) | (index >= _metaInfo.count()))
        return;

    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:property:set";
    QVariantMap parameters;
    parameters[QStringLiteral("data")] = val;
    parameters[QStringLiteral("uuid")] = _metaInfo.at(index).uuid;
    parameters[QStringLiteral("index")] = index;
    parameters[QStringLiteral("property")] = property;
    msg[QStringLiteral("parameters")]  = parameters;
    _communicationHandler->sendMessage(msg);
}

int SynchronizedListLogic::checkAndCorrectIndex(int index, QString uuid)
{
    if(uuid.isEmpty())
        return index;

    // let's hope that the order of items in the list didn't changed
    if(index >= 0 && _metaInfo.count() > index)
    {
        if(_metaInfo.at(index).uuid == uuid)
        {
            return index;
        }
    }

    // need to search the appropriate index manually :(
    QListIterator<MetaInfo> it(_metaInfo);
    int i = 0;
    while(it.hasNext())
    {
        if(it.next().uuid == uuid)
        {
            return i;
        }
        i++;
    }
    return -1;
}

void SynchronizedListLogic::setPreloadCount(int preloadCount)
{
    _preloadCount = preloadCount;
}

int SynchronizedListLogic::getRemoteItemCount() const
{
    return _remoteItemCount;
}

void SynchronizedListLogic::loadItems(int from, int count)
{
    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:get";
    QVariantMap parameters;
    parameters[QStringLiteral("from")] = from;
    parameters[QStringLiteral("count")] = count;
    msg[QStringLiteral("parameters")]  = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListLogic::fetchMore()
{
    int diff = _remoteItemCount -  _metaInfo.count();
    if(diff < _preloadCount)
        loadItems(_metaInfo.count(), diff);
    else
        loadItems(_metaInfo.count(), _preloadCount);
}


void SynchronizedListLogic::requestDump()
{
    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:dump";
    _communicationHandler->sendMessage(msg);
}

bool SynchronizedListLogic::getInitialized()
{
    return _initialized;
}


ResourceCommunicationHandler::ModelState SynchronizedListLogic::getModelState() const
{
    return _communicationHandler->getState();
}

QVariantMap SynchronizedListLogic::getMetadata() const
{
    return _metadata;
}

void SynchronizedListLogic::setMetadata(const QVariantMap &metadata)
{
    QVariantMap msg;
    msg[QStringLiteral("command")] = "synclist:metadata:set";
    QVariantMap parameters;
    parameters[QStringLiteral("metadata")] = metadata;
    msg[QStringLiteral("parameters")]  = parameters;
    _communicationHandler->sendMessage(msg);
}

QString SynchronizedListLogic::getResource() const
{
    return _resource;
}

void SynchronizedListLogic::setResource(const QString &resourceName)
{
    _resource = resourceName;
    _initialized = false;
    Q_EMIT initializedChanged(false);
    this->clearAll();
    _communicationHandler->setDescriptor(_resource);
    _communicationHandler->attachModel();
}


void SynchronizedListLogic::insertMulti(QVariantList items, int index)
{
    int i;
    if(index >= 0 && index < _metaInfo.count())
    {
        i = index;
    }
    else
    {
        if(_metaInfo.isEmpty())
            i = 0;
        else
            i = _metaInfo.count() -1;
    }

    int startIndex = i;
    QVariantList data;
    QListIterator<QVariant> it(items);
    while(it.hasNext())
    {
        QVariantMap item = it.next().toMap();
        MetaInfo info;
        info.user = item[QStringLiteral("userid")].toString();
        info.lastUpdate = item[QStringLiteral("userid")].toLongLong();
        info.uuid = item[QStringLiteral("uuid")].toString();
        _metaInfo.insert(i, info);
        data << item[QStringLiteral("data")];
        i++;
    }

    Q_EMIT itemsAdded(startIndex, data);
}

void SynchronizedListLogic::appendMulti(QVariantList items)
{
    QVariantList data;
    QListIterator<QVariant> it(items);
    while(it.hasNext())
    {
        QVariantMap item = it.next().toMap();
        MetaInfo info;
        info.user = item[QStringLiteral("userid")].toString();
        info.lastUpdate = item[QStringLiteral("userid")].toLongLong();
        info.uuid = item[QStringLiteral("uuid")].toString();
        _metaInfo.append(info);
        data << item[QStringLiteral("data")];
    }
    Q_EMIT itemsAppended(data);
}

void SynchronizedListLogic::insertItem(QVariant item, int index)
{
    QVariantMap map = item.toMap();
    MetaInfo info;
    info.user = map[QStringLiteral("userid")].toString();
    info.lastUpdate = map[QStringLiteral("userid")].toLongLong();
    info.uuid = map[QStringLiteral("uuid")].toString();

    if(index >= 0)
        _metaInfo.insert(index, info);
    else
        _metaInfo.append(info);
    Q_EMIT itemAdded(index, map[QStringLiteral("data")]);
}

void SynchronizedListLogic::removeItem(int index)
{
    _metaInfo.removeAt(index);
    Q_EMIT itemRemoved(index);
}

void SynchronizedListLogic::updateProperty(qint64 timestamp, QString property, QVariant value, int index)
{
    _metaInfo[index].lastUpdate = timestamp;
    Q_EMIT itemPropertyChanged(index, property, value);
}

void SynchronizedListLogic::clearAll()
{
    _metaInfo.clear();
    Q_EMIT listCleared();
}

void SynchronizedListLogic::updateItem(QVariant item, int index)
{
    int i;
    if(index >= 0)
        i = index;
    else
        i = _metaInfo.count() -1;

    QVariantMap map = item.toMap();
    MetaInfo info;
    info.user = map[QStringLiteral("userid")].toString();
    info.lastUpdate = map[QStringLiteral("userid")].toLongLong();
    info.uuid = map[QStringLiteral("uuid")].toString();
    _metaInfo.replace(index, info);
    Q_EMIT itemUpdated(i, map[QStringLiteral("data")]);
}

bool SynchronizedListLogic::getConnected()
{
    return _communicationHandler->isAttached();
}

void SynchronizedListLogic::connectList()
{
    _communicationHandler->attachModel();
}

void SynchronizedListLogic::stateChanged()
{

    BaseCommunicationHandler::ModelState state = _communicationHandler->getState();
    if(state != BaseCommunicationHandler::MODEL_CONNECTED)
    {
        Q_EMIT initializedChanged(false);
        _initialized = false;
    }

    Q_EMIT modelStateChanged();
}

void SynchronizedListLogic::disconnectList()
{
    _communicationHandler->detachModel();
}

void SynchronizedListLogic::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString cmd = msg[QStringLiteral("command")].toString();
    bool wasSender = msg[QStringLiteral("reply")].toBool();
    QVariantMap parameters = msg[QStringLiteral("parameters")].toMap();
    QVariant data = parameters[QStringLiteral("data")];

    if(cmd == "synclist:init")
    {
        _metadata = parameters[QStringLiteral("metadata")].toMap();
        Q_EMIT metadataChanged();
        _remoteItemCount = parameters[QStringLiteral("count")].toInt();
        Q_EMIT countChanged(_remoteItemCount);
        clearAll();
        if(_remoteItemCount < 0 || _preloadCount < 0)
            requestDump();
        else
            fetchMore();
        return;
    }

    if(cmd == "synclist:dump")
    {
        QVariantList list = parameters[QStringLiteral("data")].toList();
        _remoteItemCount = list.count();
        _metadata = parameters[QStringLiteral("metadata")].toMap();
        Q_EMIT metadataChanged();
        clearAll();
        appendMulti(list);
        if(!_initialized)
        {
            _initialized = true;
            Q_EMIT initializedChanged(true);
        }
        return;
    }

    if(cmd == "synclist:get")
    {
        QVariantList list = parameters[QStringLiteral("data")].toList();
        appendMulti(list);

        if(!_initialized)
        {
            _initialized = true;
            Q_EMIT initializedChanged(true);
        }

        return;
    }

    if(cmd == "synclist:metadata:set")
    {
        _metadata = parameters["metadata"].toMap();
        Q_EMIT metadataChanged();
        return;
    }

    if(cmd == "synclist:delete")
    {
        _metadata.clear();
        Q_EMIT metadataChanged();
        clearAll();
        _remoteItemCount = 0;
        if(wasSender)
            Q_EMIT listSuccessfullModified();

        return;
    }

    if(cmd == "synclist:append")
    {
        if(_remoteItemCount == _metaInfo.count())
            insertItem(data);

        _remoteItemCount++;
        if(wasSender)
            Q_EMIT listSuccessfullModified();

        return;
    }

    if(cmd == "synclist:appendlist")
    {
        QVariantList items = data.toList();
        if(_remoteItemCount == _metaInfo.count())
            appendMulti(items);

        _remoteItemCount += items.count();
        if(wasSender)
            Q_EMIT listSuccessfullModified();

        return;
    }

    if(cmd == "synclist:insertat")
    {
        bool ok;
        int index = parameters[QStringLiteral("index")].toInt(&ok);

        if(!ok || index < 0)
        {
            qDebug()<<"SynchronizedListLogic: Invalid data.";
            return;
        }

        if(index <= _metaInfo.count())
        {
            insertItem(data, index);
        }
        else if(_metaInfo.count() == _remoteItemCount)
        {
            insertItem(data);
        }
        _remoteItemCount++;

        if(wasSender)
            Q_EMIT listSuccessfullModified();

        return;
    }

    if(cmd == "synclist:remove")
    {
        _remoteItemCount --;
        int index = parameters[QStringLiteral("index")].toInt();
        QString uuid = parameters[QStringLiteral("uuid")].toString();
        int correctIndex = checkAndCorrectIndex(index, uuid);
        if(correctIndex >= 0)
        {
            removeItem(correctIndex);
            if(wasSender)
                Q_EMIT listSuccessfullModified();

            return;
        }
    }

    if(cmd == "synclist:property:set")
    {
        int index = parameters[QStringLiteral("index")].toInt();
        QString uuid = parameters[QStringLiteral("uuid")].toString();
        qint64 lastUpdate = parameters[QStringLiteral("lastupdate")].toLongLong();
        QString property = parameters[QStringLiteral("property")].toString();
        QVariant value = parameters[QStringLiteral("data")];
        int correctIndex = checkAndCorrectIndex(index, uuid);
        if(correctIndex >= 0)
        {
            updateProperty(lastUpdate, property, value, correctIndex);
            if(wasSender)
                Q_EMIT listSuccessfullModified();

            return;
        }
    }

    if(cmd == "synclist:set")
    {
        int index = parameters[QStringLiteral("index")].toInt();
        QString uuid = parameters[QStringLiteral("uuid")].toString();
        int correctIndex = checkAndCorrectIndex(index, uuid);
        if(correctIndex >= 0)
        {
            updateItem(data, correctIndex);
            if(wasSender)
                Q_EMIT listSuccessfullModified();

            return;
        }
    }

    if(cmd == "synclist:clear")
    {
        clearAll();
        _remoteItemCount = 0;
    }
}
