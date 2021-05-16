/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef SERVICEMODEL_H
#define SERVICEMODEL_H

#include <QObject>
#include "../Core/BaseCommunicationHandler.h"
#include <QMap>

/*!
    \qmltype ServiceModel
    \inqmlmodule CloudModels
    \inherits BaseCommunicationHandler
    \instantiates ServiceModel
    \brief Enables interaction with services.

    Services consist of a set of server-side RPCs.
    Each RPC call can be accompanied by a Java Script function which
    receives the return value.
*/


class ServiceModel : public BaseCommunicationHandler
{
    Q_OBJECT

    /*!
      \qmlproperty QString ServiceModel::service
      Speciefies the unique service name. Assigning a value directly establishes a connection
      to the corresponding service.
    */
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)

public:

    /*!
        \fn QString ServiceModel::call(QString name, QVariantMap arguments, QJSValue callback = QJSValue())
        Calls the corresponding function with the given parameters.  Return values can be received through the signature
        of the optional callback function.
    */
    Q_INVOKABLE QString call(QString name, QVariantMap arguments, QJSValue callback = QJSValue());

    //ctor
    explicit ServiceModel(QObject *parent = nullptr);

    //property getter & setter
    QString service() const;
    void setService(const QString &service);

signals:
    void serviceChanged();
     void answerReceived(QVariant data, QString id);

private slots:
    void newMessage(QVariant message);
    void stateChanged();

private:
    QString _service;
    QMap<QString, QJSValue> _calllbackMap;
    QList<QVariantMap> _queue;

public slots:
};

#endif // SERVICEMODEL_H
