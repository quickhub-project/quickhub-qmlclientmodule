/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#ifndef VIRTUALCONNECTION_H
#define VIRTUALCONNECTION_H

#include <QObject>
#include <QUuid>

#include "Connection.h"

class VirtualConnection : public QObject
{
    Q_OBJECT


public:
    enum ConnectionState
    {
        CONNECTING,
        CONNECTED,
        DISCONNECTING,
        DISCONNECTED
    };

    explicit        VirtualConnection(Connection* connection = nullptr);
                    ~VirtualConnection();
    explicit        VirtualConnection(QString uuid, Connection* connection = nullptr);
    QString         getUUID();

    // make connection as friend and do private
    void            deployMessage(const QVariantMap &message);
    ConnectionState getConnectionState();

public slots:
   void open();
   void close();   
   void sendVariant(const QVariant &data);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QVariant& message);

private:
    ConnectionState     _state;
    Connection*         _connection;
    QString             _uuid;
    bool                _connected;

private slots:
    void connectionConnected();
    void connectionDisconnected();
    void connectionDestroyed();

};

#endif // VIRTUALCONNECTION_H
