/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "VirtualConnection.h"


VirtualConnection::VirtualConnection(Connection* connection) : QObject(connection),
    _state(DISCONNECTED),
    _connection(connection),
    _connected(false)
{
    _uuid = QUuid::createUuid().toString();
    _connection->addVirtualConnection(this);
    connect(connection, &Connection::connected, this, &VirtualConnection::connectionConnected);
    connect(connection, &Connection::disconnected, this, &VirtualConnection::connectionDisconnected);
    connect(connection, &QObject::destroyed, this, &VirtualConnection::connectionDestroyed);
    if(_connection->isConnected())
        open();
}

VirtualConnection::~VirtualConnection()
{
    close();
}

VirtualConnection::VirtualConnection(QString uuid, Connection *connection): QObject(connection),
    _connection(connection),
    _uuid(uuid),
    _connected(false)
{
    _connection->addVirtualConnection(this);
    connect(connection, &Connection::connected, this, &VirtualConnection::connectionConnected);
    connect(connection, &Connection::disconnected, this, &VirtualConnection::connectionDisconnected);
    connect(connection, &QObject::destroyed, this, &VirtualConnection::connectionDestroyed);
}

QString VirtualConnection::getUUID()
{
    return _uuid;
}

void VirtualConnection::deployMessage(const QVariantMap &message)
{
    QString command = message["command"].toString();

    QVariantMap msg;
    if(command == "send")
    {
        Q_EMIT messageReceived(message["payload"]);
    }

    if(command == "connection:register")
    {
        msg["command"] ="connection:registered";
        msg["uuid"] = _uuid;
        if(!_connection)
            return;

        _connection->sendVariant(msg);
        _connected = true;
        _state = CONNECTED;
        Q_EMIT connected();
        return;
    }

    if(command == "connection:registered")
    {
        _connected = true;
        _state = CONNECTED;
        Q_EMIT connected();
        return;
    }

    if(command == "connection:close")
    {
        msg["command"] ="connection:closed";
        msg["uuid"] = _uuid;
        _connection->sendVariant(msg);
        _connected = false;
        _state = DISCONNECTED;
        Q_EMIT disconnected();
        return;
    }

    if(command == "connection:closed")
    {
        _connected = false;
        _state = DISCONNECTED;
        Q_EMIT disconnected();
        return;
    }
}

VirtualConnection::ConnectionState VirtualConnection::getConnectionState()
{
    return _state;
}

void VirtualConnection::open()
{
    if(!_connection | (_state == CONNECTING) | (_state == CONNECTED))
        return;

    _state = CONNECTING;
    QVariantMap msg;
    msg["command"] ="connection:register";
    msg["uuid"] = _uuid;
   _connection->sendVariant(msg);
}

void VirtualConnection::close()
{
    if(!_connection | (_state == DISCONNECTED))
        return;

    QVariantMap msg;
    msg["command"] ="connection:close";
    msg["uuid"] = _uuid;
    _connection->sendVariant(msg);
}

void VirtualConnection::sendVariant(const QVariant& data)
{
    if(!_connection | (_state != CONNECTED))
        return;

    QVariantMap msg;
    msg["payload"] = data;
    msg["uuid"] = _uuid;
    msg["command"] = "send";
    _connection->sendVariant(msg);
}

void VirtualConnection::connectionConnected()
{
    this->open();
}

void VirtualConnection::connectionDisconnected()
{
    _state = DISCONNECTED;
    _connected = false;
    Q_EMIT disconnected();
}

void VirtualConnection::connectionDestroyed()
{
    _connection = nullptr;
    _connected = false;
    _state = DISCONNECTED;
    Q_EMIT disconnected();
}
