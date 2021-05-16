/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef SYNCHRONIZEDLISTMODEL_H
#define SYNCHRONIZEDLISTMODEL_H

#include "../Core/ListModelBase.h"
#include "../Core/ResourceCommunicationHandler.h"
#include "../Shared/VirtualConnection.h"
#include <QObject>

class SynchronizedListModel : public ListModelBase<QVariant>
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString resource READ getResource WRITE setResource NOTIFY resourceChanged)
    Q_PROPERTY(bool connected READ getConnected NOTIFY connectedChanged)
    Q_PROPERTY(ResourceCommunicationHandler::ModelState state READ getModelState NOTIFY modelStateChanged)
    Q_PROPERTY(QVariantMap metadata READ getMetadata NOTIFY metadataChanged)

public:
    explicit SynchronizedListModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QString getUserIDForIndex(int index);
    Q_INVOKABLE qint64 getTimestampForIndex(int index);
    Q_INVOKABLE QString getUUIDForIndex(int index);
    Q_INVOKABLE void insertAt(int index, QVariantMap obj);
    Q_INVOKABLE void append(QObject* obj);
    Q_INVOKABLE void append(QVariantMap data);
    Q_INVOKABLE void appendList(QVariantList list);
    Q_INVOKABLE void set(int index, QVariantMap data);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void deleteList();
    Q_INVOKABLE void setProperty(int index, QString property, QVariant val);
    Q_INVOKABLE  QVariant get(int index);
    Q_INVOKABLE  int getIndexForUUID(QString uuid);

    QHash<int, QByteArray> _roles;
    bool getConnected();

    QVariantMap getMetadata() const;
    Q_INVOKABLE void setMetadata(const QVariantMap &metadata);

    ResourceCommunicationHandler::ModelState getModelState() const;

    QString getResource() const;
    void setResource(const QString &resourceName);

signals:
    void countChanged();
    void resourceChanged();
    void connectedChanged();
    void modelStateChanged();
    void metadataChanged();
    void listSuccessfullModified();

private:
    ResourceCommunicationHandler* _communicationHandler;
    int checkAndCorrectIndex(int index, QString uuid);
    QList<QVariant>     _pendingMessages;
    QVariantMap         _metadata;
    QString             _resource;

public slots:
    void disconnectList();
    void connectList();

private slots:

    void messageReceived(QVariant message);


public slots:
};

#endif // SYNCHRONIZEDLISTMODEL_H
