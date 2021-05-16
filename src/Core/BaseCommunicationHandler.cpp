/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "BaseCommunicationHandler.h"


BaseCommunicationHandler::BaseCommunicationHandler(QObject *parent) : QObject(parent),
    _modelState(MODEL_DISCONNECTED),
    _connected(false)
{
    _handle = new VirtualConnection(CloudModel::instance()->getConnection());
    connect(_handle, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(_handle,SIGNAL(messageReceived(QVariant)), this,SLOT(messageReceived(QVariant)));
    connect(_handle, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(CloudModel::instance(), SIGNAL(onStateChanged()), this, SLOT(handleServerState()));
}

BaseCommunicationHandler::~BaseCommunicationHandler()
{
    delete _handle;
    _handle = nullptr;
}


bool BaseCommunicationHandler::isAttached()
{
    return _connected;
}

BaseCommunicationHandler::ModelState BaseCommunicationHandler::getState() const
{
    return _modelState;
}

void BaseCommunicationHandler::setAttached(bool connected)
{
    _connected = connected;
    Q_EMIT attachedChanged();
}

void BaseCommunicationHandler::setModelState(ModelState state)
{
    _modelState = state;
    Q_EMIT stateChanged();
}

void BaseCommunicationHandler::attachModel()
{
    Q_EMIT ready();
}

void BaseCommunicationHandler::detachModel()
{
    Q_EMIT detach();
}


void BaseCommunicationHandler::handleServerState()
{
    CloudModel::ConnectionState state = CloudModel::instance()->getState();
    if(state == CloudModel::STATE_Authenticated)
    {
        attachModel();
    }

    if(state == CloudModel::STATE_Connected && _lastState == CloudModel::STATE_Authenticated)
    {
        detachModel();
    }

    _lastState = state;
}


bool BaseCommunicationHandler::sendMessage(const QVariantMap &msg)
{
    if(CloudModel::instance()->getState() >= CloudModel::STATE_Connected)
    {
        QVariantMap map = msg;
        map.insert("token",CloudModel::instance()->getToken());
       _handle->sendVariant(map);
       return true;
    }

    return false;
}

void BaseCommunicationHandler::socketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    setModelState(MODEL_ERROR);
    setAttached(false);
}

void BaseCommunicationHandler::socketDisconnected()
{
    setModelState(MODEL_DISCONNECTED);
    setAttached(false);
}

void BaseCommunicationHandler::socketConnected()
{
    setModelState(MODEL_READY);
}

void BaseCommunicationHandler::messageReceived(QVariant message)
{
    Q_EMIT newMessage(message);
}
