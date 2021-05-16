/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "Connection.h"
#include "VirtualConnection.h"
#include <QDebug>
#include <QJsonDocument>

void Connection::sendVariant(const QVariant& data)
{
    _socket->sendBinaryMessage(QJsonDocument::fromVariant(data.toMap()).toJson(QJsonDocument::Compact));
}

Connection::Connection(QWebSocket *socket, QObject *parent): QObject(parent),
    _connected(false)
{
    setSocket(socket);
}

Connection::Connection(QObject *parent) : QObject(parent),
    _connected(false)
{
}

Connection::~Connection()
{
    if(_socket)
        _socket->close();

    qDebug()<<"Connection deleted";
}

void Connection::connect(QString ident)
{
    if(_socket)
        _socket->open(ident);
}

void Connection::disconnect()
{
    if(_socket)
        _socket->close();
}

QWebSocket* Connection::getSocket()
{
    return _socket;
}

void Connection::reset()
{
    if(_connected)
    {
        Q_EMIT disconnected();
        Q_EMIT connected();
    }
}

void Connection::addVirtualConnection(VirtualConnection *connection)
{
    _handles.insert(connection->getUUID(), connection);
    QObject::connect(connection, &VirtualConnection::destroyed, this, &Connection::handleDeleted);
}

bool Connection::isConnected()
{
    return _connected;
}

void Connection::setSocket(QWebSocket *socket)
{
    if(_socket)
    {
        QObject::disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)));
        QObject::disconnect(_socket, &QWebSocket::connected, this, &Connection::socketConnected);
        QObject::disconnect(_socket, &QWebSocket::disconnected, this, &Connection::socketDisconnected);
        QObject::disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));
        QObject::disconnect(_socket,SIGNAL(binaryMessageReceived(QByteArray)), this,SLOT(messageReceived(QByteArray)));
        #ifndef WEB_ASSEMBLY
        QObject::disconnect(_socket, &QWebSocket::sslErrors, this, &Connection::sslErrors);
        typedef void (QWebSocket:: *sslErrorsSignal)(const QList<QSslError> &);
        QObject::disconnect(_socket, static_cast<sslErrorsSignal>(&QWebSocket::sslErrors),
                this, &Connection::sslErrors);
        #endif
    }

    if(socket == nullptr)
    {
        if(_connected)
        {
            _connected = false;
            Q_EMIT disconnected();
         }
        return;
    }

    socket->setParent(this);
    _socket = socket;
    QObject::connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)));
    QObject::connect(_socket, &QWebSocket::connected, this, &Connection::socketConnected);
    QObject::connect(_socket, &QWebSocket::disconnected, this, &Connection::socketDisconnected);
    QObject::connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));
    QObject::connect(_socket,SIGNAL(binaryMessageReceived(QByteArray)), this,SLOT(messageReceived(QByteArray)));
    #ifndef WEB_ASSEMBLY
    QObject::connect(_socket, &QWebSocket::sslErrors, this, &Connection::sslErrors);
    typedef void (QWebSocket:: *sslErrorsSignal)(const QList<QSslError> &);
    QObject::connect(_socket, static_cast<sslErrorsSignal>(&QWebSocket::sslErrors),
            this, &Connection::sslErrors);
    #endif

    bool isConnected = _socket->state() > 0;
    if(isConnected != _connected)
    {
        _connected = isConnected;
        if(_connected)
            Q_EMIT connected();
        else
            Q_EMIT disconnected();
    }
}

void Connection::socketDisconnected()
{
    _connected = false;
    Q_EMIT disconnected();
}

void Connection::socketConnected()
{
    _connected = true;
    if(_keepAlive)
        _keepAliveTimer->start();
    Q_EMIT connected();
}

void Connection::handleDeleted()
{
  //  qDebug()<<"Handle Deleted!";
    VirtualConnection* connection = static_cast<VirtualConnection*>(sender());
    QString key = _handles.key(connection,"");
    if(key != "")
        _handles.remove(key);
}

void Connection::messageReceived(QByteArray message)
{
   QJsonParseError error;
   QVariantMap msg = QJsonDocument::fromJson(message, &error).toVariant().toMap();
   if(error.error != QJsonParseError::NoError)
   {
       qDebug()<<"Connection: Inavlid Json.";
       return;
   }

   if(_keepAlive)
   {
       _timeoutTimer->stop();
       _keepAliveTimer->start();
       if(msg["command"] == "pong")
           return;
   }

   if(msg["command"] == "ping")
   {
       QVariantMap pong;
       pong["command"] = "pong";
       sendVariant(pong);
   }

   QString uuid = msg["uuid"].toString();
   if(_handles.contains(uuid))
   {
       _handles.value(uuid)->deployMessage(msg);
   }
   else if(msg["command"].toString()=="connection:register")
   {
       VirtualConnection* vconnection = new VirtualConnection(uuid, this);
       vconnection->deployMessage(msg);
       Q_EMIT newVirtualConnection(vconnection);
   }
}


void Connection::errorSlot(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    // TODO: add logging?
}



void Connection::setKeepAlive(int interval, int timeout)
{
    _pingInterval = interval;
    _timeout = timeout;

    if(interval <= 0)
    {
        if (_keepAliveTimer)
        {
            _keepAliveTimer->stop();
            delete _keepAliveTimer;
            _keepAliveTimer = nullptr;
        }

        if(_timeoutTimer)
        {
            _timeoutTimer->stop();
            delete _timeoutTimer;
            _timeoutTimer = nullptr;
        }

        _keepAlive = false;
        return;
    }
    else
    {
        if (!_keepAliveTimer)
        {
            _keepAliveTimer = new QTimer(this);
            _keepAliveTimer->setSingleShot(true);
            QObject::connect(_keepAliveTimer, &QTimer::timeout, this, &Connection::sendPing);
        }

        if(!_timeoutTimer)
        {
            _timeoutTimer = new QTimer(this);
            _timeoutTimer->setSingleShot(true);
            QObject::connect(_timeoutTimer, &QTimer::timeout, this, &Connection::timeout);
        }

        _timeoutTimer->setInterval(_timeout);
        _keepAliveTimer->setInterval(_pingInterval);
        _keepAliveTimer->start();
        _pingInterval = interval;
        _timeout = timeout;
        _keepAlive = true;
    }
}

void Connection::sendPing()
{
    qDebug()<<"SEND PING";
    if(!_connected)
        return;

    QVariantMap ping;
    ping["command"] = "ping";
    _socket->sendBinaryMessage(QJsonDocument::fromVariant(ping).toJson());
    _timeoutTimer->start();
}

void Connection::timeout()
{
    _connected = false;
    qDebug()<<"Timeout.";
    Q_EMIT disconnected();
}


#ifndef WEB_ASSEMBLY
void Connection::sslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
    qDebug()<<Q_FUNC_INFO<<": "<<_socket->errorString();

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.

    _socket->ignoreSslErrors();
}
#endif
