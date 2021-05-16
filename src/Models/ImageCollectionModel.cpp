/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "ImageCollectionModel.h"
#include "CloudModel.h"
#include <QJsonDocument>
#include <QUrlQuery>

ImageCollectionModel::ImageCollectionModel(QObject *parent) : QAbstractListModel(parent),
    _handler(new ResourceCommunicationHandler("imgcoll", this))
{
    connect(_handler,SIGNAL(newMessage(QVariant)), this, SLOT(messageReceived(QVariant)));
    connect(_handler,SIGNAL(attachedChanged()), this, SIGNAL(connectedChanged()));
    connect(_handler,SIGNAL(stateChanged()), this, SIGNAL(modelStateChanged()));
}

QString ImageCollectionModel::resource() const
{
    return _resource;
}


bool ImageCollectionModel::getConnected() const
{
    return _handler->isAttached();
}

ResourceCommunicationHandler::ModelState ImageCollectionModel::getModelState() const
{
    return _handler->getState();
}

QHash<int, QByteArray> ImageCollectionModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UID]      = "uid";
    roles[SOURCE]   = "url";
    roles[METADATA] = "metadata";
    return roles;
}

QVariant ImageCollectionModel::data(const QModelIndex &index, int role) const
{

    if(!index.isValid())
        return QVariant();

    switch(role)
    {
        case METADATA: return _data[index.row()].toMap()["metadata"];
        case UID: return _data[index.row()].toMap()["uid"].toString();
        case SOURCE:
        {
            QString filename = _data[index.row()].toMap()["uid"].toString();
            QString host = CloudModel::instance()->getServer().split("://").at(1);
            host = host.split(":").first();
            QUrl url;
            url.setHost("127.0.0.1");
            url.setScheme("http");
            url.setPort(8080);
            QString path = _resource;
            path = path.replace("/",".");
            path ="/images/"+path+"/"+filename;

            url.setPath(path);
            QUrlQuery query;
            query.addQueryItem("token", CloudModel::instance()->getToken());
            url.setQuery(query);
            qDebug()<<url;
            return url;

        }
    }

    return QVariant();
}

int ImageCollectionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _data.count();
}


void ImageCollectionModel::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString cmd = msg["command"].toString();

    QVariantMap parameters = msg["parameters"].toMap();
    QVariant data = parameters["data"];

    if(cmd == "imgcoll:dump")
    {
        qDebug().noquote()<<QJsonDocument::fromVariant(data).toJson();
        QVariantMap map = data.toMap();
        QMapIterator<QString, QVariant> it(map);
        Q_EMIT beginResetModel();
        while(it.hasNext())
        {
            it.next();
            QVariantMap item;
            item["uid"] = it.key();
            item["metadata"] = it.value();
            _data.append(item);
        }

        Q_EMIT endResetModel();
        return;
    }



    if(cmd == "imgcoll:new")
    {
        QVariantMap item = data.toMap();
        qDebug()<<"NEW";
        beginInsertRows(QModelIndex(),_data.count(),_data.count());
        _data.append(item);
        endInsertRows();

    }
}


void ImageCollectionModel::setResource(const QString &resourceName)
{
    if(_resource == resourceName)
        return;

    _resource = resourceName;
    _handler->setDescriptor(_resource);
    _handler->attachModel();
    Q_EMIT resourceChanged();
}
