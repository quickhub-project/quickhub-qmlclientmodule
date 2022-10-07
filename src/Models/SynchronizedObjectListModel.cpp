/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "SynchronizedObjectListModel.h"
#include <QJsonDocument>


SynchronizedObjectListModel::SynchronizedObjectListModel(QObject *parent) : QObject(parent),
    _logic(new SynchronizedListLogic(this))
{
    connect(_logic, &SynchronizedListLogic::listCleared, this, &SynchronizedObjectListModel::listCleared);
    connect(_logic, &SynchronizedListLogic::itemsAppended, this, &SynchronizedObjectListModel::itemsAppended);
    connect(_logic, &SynchronizedListLogic::itemAdded, this, &SynchronizedObjectListModel::itemAdded);
    connect(_logic, &SynchronizedListLogic::itemPropertyChanged, this, &SynchronizedObjectListModel::itemPropertyChanged);
    connect(_logic, &SynchronizedListLogic::itemRemoved, this, &SynchronizedObjectListModel::itemRemoved);
    connect(_logic, &SynchronizedListLogic::initializedChanged, this, &SynchronizedObjectListModel::initializedChanged);
}


void SynchronizedObjectListModel::itemAdded(int index, QVariant data)
{
    QVariantMap map = data.toMap();
    QString key = map[_lookupKey].toString();
    if(index < 0)
        _keyList.append(key);
    else
        _keyList.insert(index, key);
    QQmlPropertyMap*  propertymap = _nullItems.value(key, nullptr);
    _map.insert(key, initPropertyMap(map, propertymap));
    _nullItems.remove(key);
    Q_EMIT keysChanged();
    Q_EMIT sigItemAdded(key, map);
}

void SynchronizedObjectListModel::itemRemoved(int index)
{
    if(index >= _keyList.count())
        return;

    QString key = _keyList.at(index);
    _keyList.removeAt(index);

    auto map = _map.value(key);
    if(!_keepDeletedItems)
    {
        if(map != nullptr)
        {
            delete map;
            map = nullptr;
        }
    }
    else
    {
        map->insert("_exists", false);
        _nullItems.insert(key, map);
    }

    _map.remove(key);
    Q_EMIT keysChanged();
    Q_EMIT sigItemRemoved(key);
}

void SynchronizedObjectListModel::listCleared()
{
    _keyList.clear();
    QMapIterator<QString, QQmlPropertyMap*> mapIt(_map);
    while(mapIt.hasNext())
    {
        mapIt.next();
        Q_EMIT sigItemRemoved(mapIt.key());
        delete mapIt.value();
    }
    _map.clear();
    Q_EMIT keysChanged();

//    QMapIterator<QString, QQmlPropertyMap*> nullIt(mapIt);
//    while(nullIt.hasNext())
//    {
//        delete nullIt.value();
//    }
//    _nullItems.clear();

}

QQmlPropertyMap *SynchronizedObjectListModel::initPropertyMap(QVariantMap item, QQmlPropertyMap *map)
{
    if(map == nullptr)
    {
        map = new QQmlPropertyMap(this);
        connect(map, &QQmlPropertyMap::valueChanged, this, &SynchronizedObjectListModel::mapValueChanged);
    }

    QMapIterator<QString, QVariant> it(item);
    while (it.hasNext())
    {
        it.next();
        map->insert(it.key(), it.value());
    }
    map->insert("_exists", true);
    return map;
}

QObject *SynchronizedObjectListModel::getItem(QString key)
{
    QQmlPropertyMap* map = _map.value(key);
    if(map == nullptr)
    {
        map = _nullItems.value(key);
        if(map == nullptr)
        {
            map = new QQmlPropertyMap(this);
            connect(map, &QQmlPropertyMap::valueChanged, this, &SynchronizedObjectListModel::mapValueChanged);
            if(_map.count() > 0)
            {
                // init property map with the appropriate properties
                // this allows setup bindings even this item doesn't exist on serverside
                QStringList keys = _map.first()->keys();
                map->insert("_exists", false);
                foreach(QString key, keys)
                {
                    map->insert(key, QVariant());
                }
            }
            _nullItems.insert(key, map);
        }
    }

    return map;
}

bool SynchronizedObjectListModel::contains(QString key)
{
    return _map.contains(key);
}

void SynchronizedObjectListModel::insert(QObject *obj)
{
    int index = _keyList.indexOf(obj->property(_lookupKey.toLatin1()).toString());
    if(index < 0)
        _logic->append(obj);
}

void SynchronizedObjectListModel::insert(QVariantMap data)
{
    QString key = data[_lookupKey].toString();
    int index = _keyList.indexOf(key);

    if(!contains(key))
        _logic->append(data);
    else
    {
        _logic->set(index, data);
    }
}

bool SynchronizedObjectListModel::deleteItem(QString key)
{

    int i = _keyList.indexOf(key);
    if (i < 0)
    {
        return false;
    }

    _logic->remove(i);
    return true;
}

void SynchronizedObjectListModel::componentComplete()
{
    if(!_filter.isEmpty())
        _logic->setResource(_resourceName + ":" + QJsonDocument::fromVariant(_filter).toJson(QJsonDocument::Compact));
    else
        _logic->setResource(_resourceName);
    _complete = true;
}

QString SynchronizedObjectListModel::getLookupKey() const
{
    return _lookupKey;
}

void SynchronizedObjectListModel::setLookupKey(const QString &lookupKey)
{
    if(_lookupKey == lookupKey)
        return;

    _lookupKey = lookupKey;
    Q_EMIT lookupKeyChanged();
}


QVariantMap SynchronizedObjectListModel::getFilter() const
{
    return _filter;
}

void SynchronizedObjectListModel::setFilter(const QVariantMap &filter)
{
    _filter = filter;
    if(!_complete)
        return;

    _logic->setFilter(filter);
    Q_EMIT filterChanged();
}

QString SynchronizedObjectListModel::getResource() const
{
    return _logic->getResource();
}

void SynchronizedObjectListModel::setResource(const QString &resourceName)
{
    QMapIterator<QString, QQmlPropertyMap*> itA(_map);
    while(itA.hasNext())
    {
        delete itA.next().value();
    }

    QMapIterator<QString, QQmlPropertyMap*> itB(_nullItems);
    while(itB.hasNext())
    {
        delete itB.next().value();
    }

    _map.clear();
    _nullItems.clear();
    _keyList.clear();
    Q_EMIT keysChanged();
    _resourceName = resourceName;
    if(!_complete)
        return;

    _logic->setResource(resourceName);
}

QStringList SynchronizedObjectListModel::keys() const
{
    return _keyList;
}

bool SynchronizedObjectListModel::getInitialized()
{
    return _logic->getInitialized();
}

int SynchronizedObjectListModel::getCount() const
{
    return _keyList.count();
}

void SynchronizedObjectListModel::itemsAppended(QVariantList items)
{
    QListIterator<QVariant> it (items);
    while(it.hasNext())
    {
        QVariantMap item = it.next().toMap();
        QString key = item[_lookupKey].toString();
        _keyList.append(key);
        _map.insert(key, initPropertyMap(item,  _nullItems.value(key, nullptr)));
        _nullItems.remove(key);
    }

    Q_EMIT keysChanged();
}

void SynchronizedObjectListModel::mapValueChanged(const QString &key, const QVariant &input)
{
    QQmlPropertyMap* ptr = qobject_cast<QQmlPropertyMap*>(sender());
    if(ptr == nullptr)
        return;

    QString lookupKey = ptr->value(_lookupKey).toString();
    int index = _keyList.indexOf(lookupKey);
    if(index < 0)
        return;

    _logic->setProperty(index, key, input);
}

void SynchronizedObjectListModel::itemPropertyChanged(int index, QString property, QVariant data)
{
    QString key = _keyList.at(index);
    QQmlPropertyMap* map = _map.value(key, nullptr);
    if(map == nullptr)
        return;

    map->insert(property.toLatin1(), data);
}

