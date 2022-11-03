/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "../Core/ResourceCommunicationHandler.h"

class AbstractListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)


public:
    virtual QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent) const;
    Q_INVOKABLE QVariantList getListData();
    bool initialized() const;

protected:
    AbstractListModel(QObject* parent);
    void setDescriptor(QString descriptor);
    virtual void messageReceived(QVariant message);
    bool sendMessage(QVariantMap msg);
    QVariantList                    _listData;

private slots:
    void messageHandler(QVariant message);
    void attachedChanged();

private:
    ResourceCommunicationHandler*   _communicationHandler;
    bool                            _initialized = false;

signals:
    void itemAdded(int idx, QVariantMap item);
    void itemRemoved(int idx, QVariantMap item);
    void initializedChanged();
};

#endif // LISTMODEL_H
