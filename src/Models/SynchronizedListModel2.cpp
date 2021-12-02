/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include <QMetaProperty>
#include <QDebug>
#include <QJsonDocument>

#include "SynchronizedListModel2.h"
#include "../Core/CloudModel.h"


SynchronizedListModel2::SynchronizedListModel2(QObject *parent) : ListModelBase<QVariant>(parent),
    _logic(new SynchronizedListLogic(this))
{
    connect(_logic, &SynchronizedListLogic::resourceChanged, this, &SynchronizedListModel2::resourceChanged);
    connect(_logic, &SynchronizedListLogic::connectedChanged, this, &SynchronizedListModel2::connectedChanged);
    connect(_logic, &SynchronizedListLogic::modelStateChanged, this, &SynchronizedListModel2::modelStateChanged);
    connect(_logic, &SynchronizedListLogic::metadataChanged, this, &SynchronizedListModel2::metadataChanged);
    connect(_logic, &SynchronizedListLogic::listSuccessfullModified, this, &SynchronizedListModel2::listSuccessfullModified);
    connect(_logic, &SynchronizedListLogic::itemPropertyChanged, this, &SynchronizedListModel2::itemPropertyChanged);
    connect(_logic, &SynchronizedListLogic::itemUpdated, this, &SynchronizedListModel2::itemUpdated);
    connect(_logic, &SynchronizedListLogic::itemAdded, this, &SynchronizedListModel2::itemAdded);
    connect(_logic, &SynchronizedListLogic::itemRemoved, this, &SynchronizedListModel2::itemRemoved);
    connect(_logic, &SynchronizedListLogic::itemsAppended, this, &SynchronizedListModel2::itemsAppended);
    connect(_logic, &SynchronizedListLogic::listCleared, this, &SynchronizedListModel2::listCleared);
    connect(_logic, &SynchronizedListLogic::initializedChanged, this, &SynchronizedListModel2::initializedChanged);
}


QVariant SynchronizedListModel2::data(const QModelIndex &index, int role) const
{
    if(index.row() < m_dataList.size())
    {
        QVariantMap map = m_dataList.at(index.row()).toMap();

        if(role == Qt::DisplayRole)
        {
            QVariant val = map[roleNames().value(index.column()+1)];

            return val.toString();
        }

        return map[roleNames().value(role)];
    }

    return QVariant();
}

QHash<int, QByteArray> SynchronizedListModel2::roleNames() const
{
    if(m_dataList.count() > 0)
    {
        QVariantMap map = m_dataList.at(0).toMap();
        if(_roles.count() == map.count() + 1)
        {
            return _roles;
        }

        _roles.insert(Qt::DisplayRole, "display");
        for(int i = 0; i < map.keys().size(); i++)
        {
            QByteArray key =  map.keys().at(i).toLatin1();
            if(!_roles.values().contains(key))
                _roles.insert(_roles.count(), key);
        }
    }

    return _roles;
}

QString SynchronizedListModel2::getUserIDForIndex(int index)
{
    return _logic->getUserIDForIndex(index);
}

qint64 SynchronizedListModel2::getTimestampForIndex(int index)
{
    return _logic->getTimestampForIndex(index);
}

QString SynchronizedListModel2::getUUIDForIndex(int index)
{
    return _logic->getUUIDForIndex(index);
}

void SynchronizedListModel2::insertAt(int index, QVariantMap obj)
{
    _logic->insertAt(index, obj);
}

void SynchronizedListModel2::append(QObject *obj)
{
    _logic->append(obj);
}

void SynchronizedListModel2::append(QVariantMap data)
{
    _logic->append(data);
}

void SynchronizedListModel2::appendList(QVariantList list)
{
    _logic->appendList(list);
}

void SynchronizedListModel2::set(int index, QVariantMap data)
{
    _logic->set(index, data);
}

void SynchronizedListModel2::clear()
{
    _logic->clear();
}

QVariant SynchronizedListModel2::get(int index)
{
    if(index < m_dataList.size() && index >= 0)
    {
        return  m_dataList.at(index);
    }

    return QVariant();
}

int SynchronizedListModel2::getIndexForUUID(QString uuid)
{
    return _logic->getIndexForUUID(uuid);
}


void SynchronizedListModel2::componentComplete()
{
    if(!_filter.isEmpty())
        _logic->setResource(_resourceName + ":" + QJsonDocument::fromVariant(_filter).toJson(QJsonDocument::Compact));
    else
        _logic->setResource(_resourceName);
    _complete = true;
}


void SynchronizedListModel2::remove(int index)
{
    _logic->remove(index);
}

void SynchronizedListModel2::deleteList()
{
    _logic->deleteList();
}

void SynchronizedListModel2::setProperty(int index, QString property, QVariant val)
{
    _logic->setProperty(index, property, val);
}

ResourceCommunicationHandler::ModelState SynchronizedListModel2::getModelState() const
{
    return _logic->getModelState();
}

QVariantMap SynchronizedListModel2::getMetadata() const
{
    return _logic->getMetadata();
}

void SynchronizedListModel2::setMetadata(const QVariantMap &metadata)
{
    _logic->setMetadata(metadata);
}

bool SynchronizedListModel2::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    int remoteCount = _logic->getRemoteItemCount();
    if(remoteCount < 0)
      return false;

   return remoteCount > this->count();
}

void SynchronizedListModel2::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent)
    _logic->fetchMore();
}

QString SynchronizedListModel2::getResource() const
{
    return _logic->getResource();
}

void SynchronizedListModel2::setResource(const QString &resourceName)
{
    _resourceName = resourceName;
    if(!_complete)
        return;

    _logic->setResource(resourceName);
}

QVariantMap SynchronizedListModel2::getFilter() const
{
    return _filter;
}

void SynchronizedListModel2::setFilter(const QVariantMap &filter)
{
    _filter = filter;
    if(!_complete)
        return;

    _logic->setFilter(filter);
    Q_EMIT filterChanged();
}

int SynchronizedListModel2::preloadCount() const
{
    return _preloadCount;
}

void SynchronizedListModel2::setPreloadCount(int preloadCount)
{
    if(_preloadCount == preloadCount)
        return;

    _logic->setPreloadCount(preloadCount);
    _preloadCount = preloadCount;
    Q_EMIT preloadCountChanged();
}

bool SynchronizedListModel2::getInitialized()
{
    return _logic->getInitialized();
}

bool SynchronizedListModel2::getConnected()
{
    return _logic->getConnected();
}

void SynchronizedListModel2::connectList()
{
    _logic->connectList();
}

void SynchronizedListModel2::itemPropertyChanged(int index, QString property, QVariant data)
{
    if(index >= 0 && index < m_dataList.count())
    {
        QVariantMap item = m_dataList.at(index).toMap();
        item[property] = data;
        m_dataList.replace(index, item);
        int role = roleNames().key(property.toLatin1(), -1);
        QVector<int> roles;
        roles << Qt::DisplayRole;
        roles << role;
        Q_EMIT dataChanged(this->index(index), this->index(index), roles);
        Q_EMIT listModified();
    }
}

void SynchronizedListModel2::itemUpdated(int index, QVariant data)
{
    ListModelBase::replaceItem(index, data);
    Q_EMIT listModified();
}

void SynchronizedListModel2::itemAdded(int index, QVariant data)
{
    if(index < 0)
    {
        ListModelBase::appendRow(data);
        Q_EMIT sigItemAdded(this->count()-1, data);
    }
    else
    {
        ListModelBase::insertRow(index, data);
        Q_EMIT sigItemAdded(index, data);
    }
    Q_EMIT countChanged();
    Q_EMIT listModified();
}

void SynchronizedListModel2::itemRemoved(int index)
{
    ListModelBase::takeRow(index);
    Q_EMIT sigItemRemoved(index);
    Q_EMIT countChanged();
    Q_EMIT listModified();
}

void SynchronizedListModel2::listCleared()
{
    _roles.clear();
    ListModelBase::clear();
    Q_EMIT countChanged();
    Q_EMIT listModified();
}

void SynchronizedListModel2::itemsAppended(QVariantList items)
{
    ListModelBase::appendRows(items);
    Q_EMIT countChanged();
    Q_EMIT listModified();
}

void SynchronizedListModel2::disconnectList()
{
    _logic->disconnectList();
}


int SynchronizedListModel2::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED( parent );
    int roleCount = roleNames().count();
    if(roleCount <= 0)
        return 0;

    return roleCount-1;
}
