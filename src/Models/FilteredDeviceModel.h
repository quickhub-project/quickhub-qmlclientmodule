/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef FILTEREDDEVICEMODEL_H
#define FILTEREDDEVICEMODEL_H

#include <QObject>
#include "DeviceAdapterModel.h"
#include "DeviceHandleListModel.h"
/*!
    \qmltype FilteredDeviceModel
    \inqmlmodule QuickHub
    \inherits DeviceAdapterModel
    \instantiates FilteredDeviceModel
    \brief Provides a ListModel for all devices that match the specified types

    This class is an auxiliary class, and provides a ListModel for all devices that match the specified types.
    If you want to show a list with devices of a distinct type, use a instance of this class as model for your
    ListView.

    The properties of these models can be accessed from the model delegates.
    Just use the property name with a prefixed underscore.

    for example, the property "displayName" can accessed via _displayName in a binding of a delegate.
*/


class FilteredDeviceModel : public DeviceAdapterModel
{


    Q_OBJECT

    /*!
        \qmlproperty QStringList FilteredDeviceModel::deviceType
        This can be used to define one or more device types that are to
        appear later as list entries. Since device-types are often
        specified similar to path names (e.g. sockets/230vSocket or
        sockets/400vSocket) wildcards can be used. For example
        "sockets"
    */
    Q_PROPERTY(QStringList deviceType READ getDeviceType WRITE setDeviceType NOTIFY deviceTypeChanged)

    /*!
        \qmlproperty int FilteredDeviceModel::initializedCount
        The filtered device model will instanciate DeviceModels internally.
        initializedCount holds the number of fully initialized DeviceModel instances.
    */
    Q_PROPERTY(int initializedCount READ getInitializedCount NOTIFY initializedCountChanged)


public:
    QStringList getDeviceType();
    void setDeviceType(const QStringList& deviceType);
    FilteredDeviceModel(QObject* parent = nullptr);
    Q_INVOKABLE DeviceModel* getDeviceModel(QString key);
    Q_INVOKABLE QString      getMapping(int index);
    Q_INVOKABLE int          getIndexForMapping(QString mapping);
    Q_INVOKABLE QStringList  getKeys(QString type = "");

    int getInitializedCount() const;


private:
    QStringList              _deviceType;
    DeviceHandleListModel*  _deviceHandleListModel;
    QVariantList            _deviceHandleList;
    QStringList             _mappings;
    QMap<QString, DeviceModel*> _models;
    DeviceModel* insertModel(QVariantMap modelData);
    int                     _initializedCount;
    bool                    _initialized = false;

signals:
    void deviceTypeChanged();
    void initializedCountChanged();
    void initializedChanged();

public slots:
    void modelReset();

private slots:
    void itemInserted(int idx, QVariantMap data);
    void itemRemoved(int idx, QVariantMap data);
    void registerDevice(DeviceModel* model);
};

#endif // FILTEREDDEVICEMODEL_H
