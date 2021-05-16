/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "ResourceCommunicationHandler.h"


ResourceCommunicationHandler::ResourceCommunicationHandler(QString resourceType, QObject *parent) : BaseCommunicationHandler(parent),
    _resourceType(resourceType)
{

    connect(this, &BaseCommunicationHandler::stateChanged, this, &ResourceCommunicationHandler::stateChangedSlot);
}

QString ResourceCommunicationHandler::getDescriptor() const
{
    return _descriptor;
}

void ResourceCommunicationHandler::setDescriptor(const QString &resourceName)
{
    if(_descriptor == resourceName)
        return;

    _descriptor = resourceName;
    Q_EMIT descriptorChanged();
}

void ResourceCommunicationHandler::attachModel()
{
    if(_descriptor.isEmpty() )
    {
        qDebug()<<Q_FUNC_INFO<< "- No resource name";
        _attachedDescriptor = "";
        detachModel();
        return;
    }

    if(getState() == MODEL_CONNECTED)
    {
        if(_attachedDescriptor != _descriptor)
        {
            if(!_attachedDescriptor.isEmpty())
                detachModel();
            _doReconnect = true;
        }
        return;
    }


    if((getState() == MODEL_ERROR || getState() == MODEL_READY )&& CloudModel::instance()->getState() == CloudModel::STATE_Authenticated)
        p_attachModel();
    else
    {
        _attachWhenReady = true;
    }
}

void ResourceCommunicationHandler::p_attachModel()
{
    if(_descriptor.isEmpty())
        return;

    setModelState(MODEL_CONNECTING);
    QVariantMap msg;
    msg["command"] = _resourceType+":attach";
    QVariantMap payload;
    payload["descriptor"] = _descriptor;
    msg["payload"] = payload;
    sendMessage(msg);
}


void ResourceCommunicationHandler::detachModel()
{
    QVariantMap msg;
    msg["command"] =  _resourceType+":detach";
    sendMessage(msg);
}

void ResourceCommunicationHandler::stateChangedSlot()
{
    if((getState() == MODEL_READY) && _attachWhenReady && CloudModel::instance()->getState() == CloudModel::STATE_Authenticated)
    {
          p_attachModel();
    }
}


void ResourceCommunicationHandler::messageReceived(QVariant message)
{
    QVariantMap msg = message.toMap();
    QString cmd = msg["command"].toString();
    QString msgID = msg["msguid"].toString();
    QVariantMap parameters = msg["parameters"].toMap();
    QVariant data = parameters["data"];

    // check wether the message contains a message ID. If so, send an ACK
    if(!msgID.isEmpty())
    {
        QVariantMap msg;
        msg["command"] = "ACK";
        msg["msguid"] = msgID;
        sendMessage(msg);
    }

    if(cmd == _resourceType+":attach:success")
    {
        setAttached(true);
        qDebug()<< _resourceType+":"+_descriptor+"  -> Attached.";
        Q_EMIT attachedChanged();
        setModelState(MODEL_CONNECTED);
        _attachedDescriptor = _descriptor;
        return;
    }

    if(cmd == _resourceType+":attach:failed")
    {
        setModelState(MODEL_ERROR);
        return;
    }

    if(cmd == _resourceType+":detach:success"|| cmd == _resourceType+":detached")
    {
        setModelState(MODEL_DISCONNECTED);
        qDebug()<<_resourceType+":"+_attachedDescriptor+"   -> Detached.";
        setAttached(false);
        Q_EMIT attachedChanged();
        if(_doReconnect)
            p_attachModel();
        return;
    }

    Q_EMIT newMessage(message);


}


