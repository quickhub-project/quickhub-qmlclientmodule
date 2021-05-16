/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICEADAPTERMODEL_H
#define DEVICEADAPTERMODEL_H

#include <QObject>
#include <QAbstractListModel>


/*!
    \class DeviceAdapterModel
    \brief This model takes multiple instances of DeviceModel objects and maps them to a list.
    \inherits QAbstractListModel
    This allows the usage of multiple devices in a list view or a repeater. The device properties will be accessible as roles.
    Calling "addDeviceModel" registers the model at its properties to the list model.
    \note FilteredDeviceModel shows an exmaple how to use it.
*/

class DeviceModel;
class DevicePropertyModel;
class DeviceAdapterModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(bool initialized READ getInitialized NOTIFY initializedChanged)

public:
    explicit DeviceAdapterModel(QObject *parent = nullptr);

    // QAbstractListModel API
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*!

    */
    void                    setDeviceModels(QList<DeviceModel*> models);
    void                    addDeviceModel(DeviceModel* model);
    void                    clearAll();
    bool                    removeDeviceModel(DeviceModel *model);
    Q_INVOKABLE  DeviceModel* getModelAt(int index);
    bool                    getInitialized() const;

private:
    void reIndex();
    QList<DeviceModel*>     _models;
    mutable QHash<int, QByteArray> _roles;
    /*!
        when a property of a DeviceModel changes it is expensive
        to find out which list position needs to be updated. In this map,
        the corresponding index is stored for each property object.
    */
    QMap<DevicePropertyModel* , int > _propertyToIndexMap;
    bool _initialized = false;

private slots:
    void propertyValueChanged();
    void propertiesChanged();

signals:
    void modelInitialized(DeviceModel* model);
    void countChanged();
    void initializedChanged();


public slots:
};

#endif // CONTROLLERMODEL_H
