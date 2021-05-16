/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef BASECOMMUNICATIONHANDLER_H
#define BASECOMMUNICATIONHANDLER_H

#include <QObject>
#include "../Shared/VirtualConnection.h"
#include "../Core/CloudModel.h"

class BaseCommunicationHandler : public QObject
{
    Q_OBJECT

public:
    enum ModelState
    {
        MODEL_ERROR = -1,
        MODEL_DISCONNECTED = 0,
        MODEL_READY,
        MODEL_CONNECTING,
        MODEL_CONNECTED,
        MODEL_TRANSFERRING,
    };
    Q_ENUM(ModelState)

    explicit BaseCommunicationHandler(QObject *parent = nullptr);
    ~BaseCommunicationHandler();
    bool isAttached();
    BaseCommunicationHandler::ModelState getState() const;
    void setAttached(bool connected);

protected:
    void setModelState(BaseCommunicationHandler::ModelState state);

private:
    ModelState          _modelState;
    VirtualConnection*  _handle;
    bool                _connected;
    CloudModel::ConnectionState _lastState = CloudModel::STATE_Disconnected;

public slots:
    virtual void attachModel();
    virtual void detachModel();

signals:
    void ready();
    void detach();
    void newMessage(QVariant message);
    void attachedChanged();
    void stateChanged();

private slots:
    void handleServerState();
    void socketError(QAbstractSocket::SocketError error);
    void socketDisconnected();
    void socketConnected();

protected slots:
    virtual void messageReceived(QVariant message);

public:
    bool sendMessage(const QVariantMap &msg);
};

#endif // BASECOMMUNICATIONHANDLER_H
