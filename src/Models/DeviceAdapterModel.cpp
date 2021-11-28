/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "DeviceAdapterModel.h"
#include "DeviceModel.h"
#include "DevicePropertyModel.h"
#include <QVariant>

DeviceAdapterModel::DeviceAdapterModel(QObject *parent) : QAbstractListModel(parent)
{
}

QVariant DeviceAdapterModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < _models.size())
    {
        QString key = _roles.value(role);
        key = key.remove(0,1);
        DevicePropertyModel* model =  _models.at(row)->deviceProperties().value(key);

        if(model != nullptr)
            return model->getValue();
        return QVariant();
    }

    return QVariant();
}

QHash<int, QByteArray> DeviceAdapterModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    if(!_models.isEmpty())
    {
        QStringList keys = _models.first()->deviceProperties().keys();
        if(_roles.count() == keys.count())
        {
            return _roles;
        }

        QListIterator<QString> it(keys);
        while(it.hasNext())
        {
            QByteArray key = "_"+it.next().toLatin1();
            if(!_roles.values().contains(key))
                _roles.insert(_roles.count(), key);
        }
    }
    return _roles;
}

int DeviceAdapterModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _models.count();
}

void DeviceAdapterModel::setDeviceModels(QList<DeviceModel *> models)
{
    _models.append(models);
    QListIterator<DeviceModel*> it(models);
    while(it.hasNext())
    {
        DeviceModel* model = it.next();
        connect(model, &DeviceModel::initializedChanged, this, &DeviceAdapterModel::propertiesChanged);
    }
    Q_EMIT countChanged();
}


void DeviceAdapterModel::addDeviceModel(DeviceModel *model)
{
    connect(model, &DeviceModel::initializedChanged, this, &DeviceAdapterModel::propertiesChanged);
    beginInsertRows(QModelIndex(), _models.count(), _models.count());
    _models.append(model);
    endInsertRows();
    Q_EMIT countChanged();
}

bool DeviceAdapterModel::removeDeviceModel(DeviceModel *model)
{
    int idx = _models.indexOf(model);
    if(idx < 0)
        return false;


    QList<DevicePropertyModel*> propertyModels = _propertyToIndexMap.keys(idx);
    QListIterator<DevicePropertyModel*> it(propertyModels);
    while(it.hasNext())
    {
        DevicePropertyModel* model = it.next();
        disconnect(model, &DevicePropertyModel::valueChanged, this, &DeviceAdapterModel::propertyValueChanged);
        _propertyToIndexMap.remove(model);
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    _models.removeAt(idx);
    reIndex();
    endRemoveRows();
    return true;
}


DeviceModel *DeviceAdapterModel::getModelAt(int index)
{
    if(_models.count() > index && index >= 0)
        return _models.at(index);
    return nullptr;
}

bool DeviceAdapterModel::getInitialized() const
{
    return _initialized;
}

void DeviceAdapterModel::reIndex()
{
    _propertyToIndexMap.clear();
    for(int i = 0; i < _models.length(); i++)
    {
        DeviceModel* model = _models.at(i);
        auto tempProps = model->deviceProperties().values();
        QListIterator<DevicePropertyModel*> properties(tempProps);
        while(properties.hasNext())
        {
            DevicePropertyModel* model = properties.next();
            _propertyToIndexMap.insert(model, i);
            connect(model, &DevicePropertyModel::valueChanged, this, &DeviceAdapterModel::propertyValueChanged);
        }
    }
}

void DeviceAdapterModel::propertyValueChanged()
{
    DevicePropertyModel* model = qobject_cast<DevicePropertyModel*>(sender());
   int index = _propertyToIndexMap.value(model, -1);
   if(index >= 0)
   {
       int role = roleNames().key("_"+model->getName().toLatin1(), -1);
       QVector<int> roles;
       roles << role;
       Q_EMIT dataChanged(this->index(index), this->index(index), roles);
   }
}

void DeviceAdapterModel::propertiesChanged()
{
    auto model = qobject_cast<DeviceModel*>(sender());
    int index = _models.indexOf(model);
    auto tempProps = model->deviceProperties().values();
    QListIterator<DevicePropertyModel*> properties(tempProps);
    while(properties.hasNext())
    {
        DevicePropertyModel* model = properties.next();
        _propertyToIndexMap.insert(model, index);
        connect(model, &DevicePropertyModel::valueChanged, this, &DeviceAdapterModel::propertyValueChanged);
    }

    //check if all models are initialized
    bool uninitializedEntriesFound = false;
    for(int i = 0; i < _models.length(); i++)
    {
        DeviceModel* model = _models.at(i);
        if(!model->getInitialized())
        {
            uninitializedEntriesFound = true;
            break;
        }
    }

    if(!uninitializedEntriesFound)
    {
        beginResetModel();
        endResetModel();
        _initialized = true;
        Q_EMIT initializedChanged();
    }
    Q_EMIT modelInitialized(model);
    Q_EMIT dataChanged(this->index(index), this->index(index));
}


void  DeviceAdapterModel::clearAll()
{
    QListIterator<DeviceModel*> it(_models);
    while(it.hasNext())
    {
        removeDeviceModel(it.next());
    }
}
