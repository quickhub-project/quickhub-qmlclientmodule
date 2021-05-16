/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "AbstractListModel.h"
#include <QJsonDocument>
AbstractListModel::AbstractListModel(QObject *parent) : QAbstractListModel(parent),
     _communicationHandler(new ResourceCommunicationHandler("list", this))
{
    connect(_communicationHandler, &ResourceCommunicationHandler::newMessage, this, &AbstractListModel::messageHandler);
    connect(_communicationHandler, &ResourceCommunicationHandler::attachedChanged, this, &AbstractListModel::attachedChanged);
}

QVariant AbstractListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < _listData.size())
    {
        QVariantMap map = _listData.at(index.row()).toMap();
        return map[roleNames().value(role)];
    }

    return QVariant();
}

QHash<int, QByteArray> AbstractListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    if(_listData.count() > 0)
    {
        QVariantMap map = _listData.at(0).toMap();
        for(int i = 0; i < map.keys().size(); i++)
        {
            roles.insert(i, map.keys().at(i).toLatin1());
        }
    }

    return roles;
}

int AbstractListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED (parent)
    return _listData.count();
}

QVariantList AbstractListModel::getListData()
{
    return _listData;
}

void AbstractListModel::setDescriptor(QString descriptor)
{
    _communicationHandler->setDescriptor(descriptor);
    _communicationHandler->attachModel();
}

void AbstractListModel::messageReceived(QVariant message)
{
    Q_UNUSED(message)
}

bool AbstractListModel::sendMessage(QVariantMap msg)
{
    return _communicationHandler->sendMessage(msg);
}

void AbstractListModel::messageHandler(QVariant message)
{
    QVariantMap msg = message.toMap();
    QVariantMap parameters = msg["parameters"].toMap();
    QString cmd = msg["command"].toString();


    if(cmd == "list:dump")
    {
        QVariantList list = parameters["data"].toList();
        beginResetModel();
        _listData = list;
        endResetModel();
        return;
    }

    int idx = parameters["index"].toInt();
    QVariant data = parameters["data"];

    if(cmd == "list:property:set")
    {
        QString property = parameters["property"].toString();
        if(idx >= _listData.count())
        {
            return;
        }
        QVariantMap item = _listData.at(idx).toMap();
        item[property] = data;
        _listData.replace(idx, item);
        Q_EMIT dataChanged(index(idx),index(idx));
        return;
    }

    if(cmd == "list:set")
    {
        _listData.replace(idx, data);
        Q_EMIT dataChanged(index(idx),index(idx));
        return;
    }

    if(cmd == "list:insertat")
    {
        Q_EMIT  beginInsertRows(QModelIndex(), idx, idx);
        _listData.insert(idx, data);
        Q_EMIT itemAdded(idx, data.toMap());
        Q_EMIT endInsertRows();
        return;
    }

    if(cmd == "list:remove")
    {
        Q_EMIT  beginRemoveRows(QModelIndex(), idx, idx);
        Q_EMIT itemRemoved(idx, _listData.at(idx).toMap());
        _listData.removeAt(idx);
        Q_EMIT endRemoveRows();
        return;
    }

    messageReceived(message);
}

void AbstractListModel::attachedChanged()
{
    if(_communicationHandler->isAttached())
    {
        beginResetModel();
        _listData.clear();
        endResetModel();
    }
}
