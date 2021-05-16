/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef IMAGECOLLECTIONMODEL_H
#define IMAGECOLLECTIONMODEL_H

#include <QObject>
#include "../Core/ResourceCommunicationHandler.h"
#include <QAbstractListModel>


/*!
    \qmltype ImageCollectionModel
    \inqmlmodule CloudModels
    \inherits QAbstractListModel
    \instantiates ImageCollectionModel
    \brief Provides access to QuickHub image collections

    This class is in experimental state and not officially supported.
*/

class ImageCollectionModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString resource READ resource WRITE setResource NOTIFY resourceChanged)
    Q_PROPERTY(bool connected READ getConnected NOTIFY connectedChanged)
    Q_PROPERTY(ResourceCommunicationHandler::ModelState connectionState READ getModelState NOTIFY modelStateChanged)

public:
    enum Roles
    {
        UID = Qt::DisplayRole,
        METADATA,
        SOURCE
    };

    explicit ImageCollectionModel(QObject *parent = nullptr);

    QString resource() const;
    void setResource(const QString &resource);

    bool getConnected() const;
    ResourceCommunicationHandler::ModelState getModelState() const;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

signals:
    void resourceChanged();
    void connectedChanged();
    void modelStateChanged();

private:
    ResourceCommunicationHandler* _handler = nullptr;
    QString _resource;
    QList<QVariant> _data;


private slots:
       void messageReceived(QVariant message);


public slots:
};

#endif // IMAGECOLLECTIONMODEL_H
