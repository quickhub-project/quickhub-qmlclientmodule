/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "ServiceModel.h"
#include <QJSEngine>


ServiceModel::ServiceModel(QObject *parent) : BaseCommunicationHandler(parent)
{
    connect(this, &BaseCommunicationHandler::newMessage, this, &ServiceModel::newMessage);
    connect(this, &BaseCommunicationHandler::stateChanged, this, &ServiceModel::stateChanged);
}

QString ServiceModel::call(QString name, QVariantMap arguments, QJSValue callback)
{
    QVariantMap msg;
    msg["command"] = "call:"+_service+"/"+name;
    QVariantMap payload;
    QString uuid = QUuid::createUuid().toString();
    payload["arg"] = arguments;
    payload["uid"] = uuid;
    msg["payload"] = payload;
    _calllbackMap.insert(uuid, callback);
    if(getState() != BaseCommunicationHandler::MODEL_READY)
        _queue.append(msg);
    else
        sendMessage(msg);
    return uuid;
}


QString ServiceModel::service() const
{
    return _service;
}

void ServiceModel::setService(const QString &service)
{
    _service = service;
    Q_EMIT serviceChanged();
}

void ServiceModel::newMessage(QVariant message)
{
    QVariantMap map = message.toMap();
    QString uid = map["uid"].toString();
    QJSValue cb = _calllbackMap.value(uid);
    _calllbackMap.remove(uid);
   // QJSEngine engine;
    QVariant arg = map["data"];
    Q_EMIT answerReceived(arg, uid);

    if(cb.isCallable())
    {
        auto engine = qjsEngine(this);
        cb.call( QJSValueList{ engine->toScriptValue(arg) } );
    }
}

void ServiceModel::stateChanged()
{
    if(getState() != BaseCommunicationHandler::MODEL_READY)
        return;

    for(const QVariantMap& msg : _queue)
    {
        sendMessage(msg);
    }
}
