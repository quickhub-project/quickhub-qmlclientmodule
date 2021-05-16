/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICEHANDLETREEMODEL_H
#define DEVICEHANDLETREEMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include "../Core/ResourceCommunicationHandler.h"
#include "TreeItem.h"

class DeviceHandleTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    DeviceHandleTreeModel(QObject* parent = nullptr);


    Q_INVOKABLE QVariant getData(QModelIndex index, QString role);
    virtual QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &index) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

protected:
    bool sendMessage(QVariantMap msg);
    QVariantList _listData;

private slots:
    void messageHandler(QVariant message);
    void attachedChanged();

private:
    ResourceCommunicationHandler* _communicationHandler;
    void setupModelData();
    TreeItem *rootItem;
    void addNode(QString path);

};

#endif // DEVICEHANDLETREEMODEL_H
