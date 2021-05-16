/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "FilteredDeviceModel.h"
#include "DeviceModel.h"
#include "DevicePropertyModel.h"
#include <QJsonDocument>
#include <QDebug>
#include <QRegularExpression>

FilteredDeviceModel::FilteredDeviceModel(QObject *parent) : DeviceAdapterModel (parent),
    _deviceHandleListModel(new DeviceHandleListModel(this))
{
    connect(_deviceHandleListModel, &DeviceHandleListModel::modelReset, this, &FilteredDeviceModel::modelReset);
    connect(_deviceHandleListModel, &DeviceHandleListModel::itemAdded, this, &FilteredDeviceModel::itemInserted);
    connect(_deviceHandleListModel, &DeviceHandleListModel::itemRemoved, this, &FilteredDeviceModel::itemRemoved);
    connect(this, &DeviceAdapterModel::modelInitialized, this, &FilteredDeviceModel::registerDevice);
}

DeviceModel *FilteredDeviceModel::getDeviceModel(QString key)
{
    return _models.value(key, nullptr);
}

QString FilteredDeviceModel::getMapping(int index)
{
    if(index < 0 | index >= _mappings.count())
        return "";

    return _mappings.at(index);
}

int FilteredDeviceModel::getIndexForMapping(QString mapping)
{
    return _mappings.indexOf(mapping);
}

/*
    take the mapping, setup the model object and add it to the DeviceAdapterModel
*/
DeviceModel* FilteredDeviceModel::insertModel(QVariantMap modelData)
{
    QString type = modelData["type"].toString();
    bool hasMatch = false;
    QListIterator<QString> it(_deviceType);
    while(it.hasNext())
    {
        QRegularExpression regex(QRegularExpression::wildcardToRegularExpression(it.next()));
        if(regex.match(type).hasMatch())
        {
            hasMatch = true;
            break;
        }
    }

    if(hasMatch)
    {

        QVariantList mappings = modelData["mappings"].toList();
        if(!mappings.isEmpty())
        {
            DeviceModel* model = new DeviceModel(this);
            QString mapping = mappings.first().toString();
            _mappings << mapping;
            model->setResource(mapping);
            return model;
        }
    }
    return nullptr;
}

int FilteredDeviceModel::getInitializedCount() const
{
    return _models.count();
}

/*
    We need to wait until devicelistmodel receives the data.
*/
void FilteredDeviceModel::modelReset()
{
    clearAll();

    _deviceHandleList = _deviceHandleListModel->getListData();
    if(_deviceType.isEmpty())
        return;

    QList<DeviceModel*> models;
    QListIterator<QVariant> it(_deviceHandleList);
    while(it.hasNext())
    {
        QVariantMap deviceHandle = it.next().toMap();
        DeviceModel* model = insertModel(deviceHandle);
        if(model)
            models << model;
    }

    if(!models.isEmpty())
        setDeviceModels(models);
}

void FilteredDeviceModel::itemInserted(int idx, QVariantMap data)
{
    Q_UNUSED(idx)
    auto model = insertModel(data);
    if(model)
        addDeviceModel(model);

}

void FilteredDeviceModel::itemRemoved(int idx, QVariantMap data)
{
    Q_UNUSED(idx)
    QString uuid = data["uuid"].toString();
    if(_models.contains(uuid))
    {
        DeviceModel* model = _models.value(uuid, nullptr);
        if(model != nullptr)
        {
            removeDeviceModel(model);
            _models.remove(uuid);
            Q_EMIT initializedCountChanged();
            delete model;
        }
    }
}

// add device to an internal hashmap
void FilteredDeviceModel::registerDevice(DeviceModel *model)
{

    QString key = model->getProperty("deviceID")->getValue().toString();
    _models.insert(key, model);
    Q_EMIT initializedCountChanged();
}

QStringList FilteredDeviceModel::getDeviceType()
{
    return _deviceType;
}

void FilteredDeviceModel::setDeviceType(const QStringList &deviceType)
{
    _deviceType = deviceType;
    Q_EMIT deviceTypeChanged();
}

QStringList  FilteredDeviceModel::getKeys(QString type)
{
    QStringList keys;
    QMapIterator<QString, DeviceModel*> it(_models);
    while(it.hasNext())
    {
        it.next();
        if(type.isEmpty() || it.value()->getType()  == type)
        {
            keys.append(it.key());
        }
    }

    return keys;
}
