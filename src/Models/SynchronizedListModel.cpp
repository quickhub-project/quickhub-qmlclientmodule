/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include <QMetaProperty>
#include <QDebug>
#include <QJsonDocument>

#include "SynchronizedListModel.h"
#include "../Core/CloudModel.h"


SynchronizedListModel::SynchronizedListModel(QObject *parent) : ListModelBase(parent),
    _communicationHandler(new ResourceCommunicationHandler("synclist", this))
{
    connect(_communicationHandler,SIGNAL(newMessage(QVariant)), this, SLOT(messageReceived(QVariant)));
    connect(_communicationHandler,SIGNAL(attachedChanged()), this, SIGNAL(connectedChanged()));
    connect(_communicationHandler,SIGNAL(stateChanged()), this, SIGNAL(modelStateChanged()));
}

QVariant SynchronizedListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < m_dataList.size())
    {
        QVariantMap map = m_dataList.at(index.row()).toMap()["data"].toMap();
        return map[roleNames().value(role)];
    }

    return QVariant();
}

QHash<int, QByteArray> SynchronizedListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    if(m_dataList.count() > 0)
    {
        QVariantMap map = m_dataList.at(0).toMap()["data"].toMap();
        for(int i = 0; i < map.keys().size(); i++)
        {
            roles.insert(i, map.keys().at(i).toLatin1());
        }
    }

    return roles;
}

QString SynchronizedListModel::getUserIDForIndex(int index)
{
    if(m_dataList.size() > index && index >= 0)
    {
        return m_dataList.at(index).toMap()["userid"].toString();
    }
    return "unknown";
}

qint64 SynchronizedListModel::getTimestampForIndex(int index)
{
    if(m_dataList.size() > index && index >= 0)
    {
        return m_dataList.at(index).toMap()["timestamp"].toLongLong();
    }
    return -1;
}

QString SynchronizedListModel::getUUIDForIndex(int index)
{
    if(m_dataList.size() > index && index >= 0)
    {
        return m_dataList.at(index).toMap()["uuid"].toString();
    }
    return "";
}

void SynchronizedListModel::insertAt(int index, QVariantMap obj)
{
    QVariantMap parameters;
    parameters["index"] = index;
    parameters["data"] = obj;

    QVariantMap msg;
    msg["command"] = "synclist:insertat";
    msg["parameters"] = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListModel::append(QObject *obj)
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

void SynchronizedListModel::append(QVariantMap data)
{
    QVariantMap parameters;
    parameters["data"] = data;

    QVariantMap msg;
    msg["command"] = "synclist:append";
    msg["parameters"] = parameters;


    _communicationHandler->sendMessage(msg);
}

void SynchronizedListModel::appendList(QVariantList list)
{
    QVariantMap parameters;
    parameters["data"] = list;

    QVariantMap msg;
    msg["command"] = "synclist:appendlist";
    msg["parameters"] = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListModel::set(int index, QVariantMap data)
{
    QVariantMap parameters;
    parameters["data"] = data;
    parameters["index"] = index;
    if(index >= 0 && index < m_dataList.count())
    {
        parameters["uuid"] = m_dataList.at(index).toMap()["uuid"];
    }

    QVariantMap msg;
    msg["command"] = "synclist:set";
    msg["parameters"] = parameters;
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListModel::clear()
{
    QVariantMap msg;
    msg["command"] = "synclist:clear";
    _communicationHandler->sendMessage(msg);
}

QVariant SynchronizedListModel::get(int index)
{
    if(index < m_dataList.size() && index >= 0)
    {
        return  m_dataList.at(index).toMap()["data"];
    }

    return -1;
}

int SynchronizedListModel::getIndexForUUID(QString uuid)
{
    return checkAndCorrectIndex(-1, uuid);
}

void SynchronizedListModel::remove(int index)
{
    if(index >= 0 && index < m_dataList.count())
    {

            QVariantMap item = m_dataList.at(index).toMap();
            QVariantMap msg;
            msg["command"] = "synclist:remove";
            QVariantMap parameters;
            parameters["uuid"] = item["uuid"];
            parameters["index"] = index;
            msg["parameters"] = parameters;
            _communicationHandler->sendMessage(msg);
    }
}

void SynchronizedListModel::deleteList()
{
    QVariantMap msg;
    msg["command"] = "synclist:delete";
    _communicationHandler->sendMessage(msg);
}

void SynchronizedListModel::setProperty(int index, QString property, QVariant val)
{
    if((index < 0) | (index >= m_dataList.count()))
        return;

    QVariantMap msg;
    msg["command"] = "synclist:property:set";
    QVariantMap parameters;
    parameters["data"] = val;
    parameters["uuid"] = m_dataList.at(index).toMap()["uuid"];
    parameters["index"] = index;
    parameters["property"] = property;
    msg["parameters"]  = parameters;
    _communicationHandler->sendMessage(msg);
}

int SynchronizedListModel::checkAndCorrectIndex(int index, QString uuid)
{
    // let's hope that the order of items in the list didn't changed
    if(index >= 0 && m_dataList.count() > index)
    {
        QVariantMap item = m_dataList.at(index).toMap();
        if(item["uuid"].toString() == uuid)
        {
            return index;
        }
    }

    // need to search the appropriate index manually :(
    QListIterator<QVariant> it(m_dataList);
    int i = 0;
    while(it.hasNext())
    {
        if(it.next().toMap()["uuid"].toString() == uuid)
        {
            return i;
        }
        i++;
    }
    return -1;
}


ResourceCommunicationHandler::ModelState SynchronizedListModel::getModelState() const
{
    return _communicationHandler->getState();
}

QVariantMap SynchronizedListModel::getMetadata() const
{
    return _metadata["metadata"].toMap();
}

void SynchronizedListModel::setMetadata(const QVariantMap &metadata)
{
    QVariantMap msg;
    msg["command"] = "synclist:metadata:set";
    QVariantMap parameters;
    parameters["metadata"] = metadata;
    msg["parameters"]  = parameters;
    _communicationHandler->sendMessage(msg);
}

QString SynchronizedListModel::getResource() const
{
    return _resource;
}

void SynchronizedListModel::setResource(const QString &resourceName)
{
    _resource = resourceName;
    _communicationHandler->setDescriptor(_resource);
    _communicationHandler->attachModel();
}

bool SynchronizedListModel::getConnected()
{
    return _communicationHandler->isAttached();
}

void SynchronizedListModel::connectList()
{
  _communicationHandler->attachModel();
}

void SynchronizedListModel::disconnectList()
{
 _communicationHandler->detachModel();
}

void SynchronizedListModel::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString cmd = msg["command"].toString();
    bool wasSender = msg["reply"].toBool();
    QVariantMap parameters = msg["parameters"].toMap();
    QVariant data = parameters["data"];

    if(cmd == "synclist:dump")
    {
        QVariantMap parameters = msg["parameters"].toMap();
        QVariantList list = parameters["data"].toList();
        _metadata = parameters["metadata"].toMap();
        Q_EMIT metadataChanged();
        beginResetModel();
        ListModelBase::clear();
        ListModelBase::appendRows(list);
        endResetModel();
        Q_EMIT countChanged();        
        return;
    }

    if(cmd == "synclist:metadata:set")
    {
        _metadata["metadata"] = parameters["metadata"];
        Q_EMIT metadataChanged();
        return;
    }

    if(cmd == "synclist:delete")
    {
        _metadata.clear();
        Q_EMIT metadataChanged();
        ListModelBase::clear();
        Q_EMIT countChanged();
        if(wasSender)
            Q_EMIT listSuccessfullModified();

        return;
    }

    if(cmd == "synclist:append")
    {
        ListModelBase::appendRow(data);
        Q_EMIT countChanged();
        if(wasSender)
            Q_EMIT listSuccessfullModified();

        return;
    }

    if(cmd == "synclist:appendlist")
    {
        QVariantList items = data.toList();
        ListModelBase::appendRows(items);
        Q_EMIT countChanged();
        if(wasSender)
            Q_EMIT listSuccessfullModified();

        return;
    }

    if(cmd == "synclist:insertat")
    {
        bool ok;
        int index = parameters["index"].toInt(&ok);
        if(!ok || index < 0)
        {
            qDebug()<<"SynchronizedListModel: Invalid data.";
            return;
        }

        if(index < m_dataList.count())
            ListModelBase::insertRow(index, data);
        else
            ListModelBase::appendRow(data);
        Q_EMIT countChanged();
        if(wasSender)
            Q_EMIT listSuccessfullModified();

        return;
    }

    if(cmd == "synclist:remove")
    {
        int index = parameters["index"].toInt();
        QString uuid = parameters["uuid"].toString();
        int correctIndex = checkAndCorrectIndex(index, uuid);
        if(correctIndex >= 0)
        {
            ListModelBase::takeRow(correctIndex);
            Q_EMIT countChanged();
            if(wasSender)
                Q_EMIT listSuccessfullModified();

            return;
        }
    }

    if(cmd == "synclist:property:set")
    {
        int index = parameters["index"].toInt();
        QString uuid = parameters["uuid"].toString();
        QString lastUpdate = parameters["lastupdate"].toString();
        QString property = parameters["property"].toString();
        QVariant value = parameters["data"];
        int correctIndex = checkAndCorrectIndex(index, uuid);
        if(correctIndex >= 0)
        {
            QVariantMap item = m_dataList[index].toMap();
            item["lastupdate"] = lastUpdate;
            QVariantMap tmp = item["data"].toMap();
            tmp[property] = value;
            item["data"] = tmp;
            int role = roleNames().key(property.toLatin1(), -1);
            if (role < 0) // invalid role - property doesn't exist
                return;

            QVector<int> roles;
            roles << role;
            m_dataList[correctIndex] = item;
            Q_EMIT dataChanged(this->index(index), this->index(index), roles);

            if(wasSender)
                Q_EMIT listSuccessfullModified();

            return;
        }
    }

    if(cmd == "synclist:set")
    {
        int index = parameters["index"].toInt();
        QString uuid = parameters["uuid"].toString();
        int correctIndex = checkAndCorrectIndex(index, uuid);
        if(correctIndex >= 0)
        {
            ListModelBase::replaceItem(correctIndex, data);
            if(wasSender)
                Q_EMIT listSuccessfullModified();

            return;
        }
    }

    if(cmd == "synclist:clear")
    {
        ListModelBase::clear();
    }
}
