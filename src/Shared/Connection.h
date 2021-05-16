/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#ifndef CONNECTIONHANDLER_H
#define CONNECTIONHANDLER_H

#include <QObject>
#include <QWebSocket>
#include <QTimer>

class VirtualConnection;
class Connection : public QObject
{
    Q_OBJECT

public:
    explicit    Connection(QWebSocket* socket, QObject *parent = nullptr);
    explicit    Connection(QObject *parent = nullptr);
                ~Connection();
    void        connect(QString ident);
    void        disconnect();
    void        sendVariant(const QVariant &data);
    void        addVirtualConnection(VirtualConnection* connection);
    void        setKeepAlive(int interval, int timeout = 1000);
    bool        isConnected();
    void        setSocket(QWebSocket* socket);
    QWebSocket* getSocket();
    void        reset();

private:
    QWebSocket*                         _socket = nullptr;
    bool                                _connected;
    QHash<QString, VirtualConnection*>  _handles;
    bool                                _keepAlive = false;
    int                                 _pingInterval;
    int                                 _timeout;
    QTimer*                             _keepAliveTimer = nullptr;
    QTimer*                             _timeoutTimer =  nullptr;


signals:
    void connected();
    void disconnected();
    void newVirtualConnection(VirtualConnection* connection);
    void socketError(QAbstractSocket::SocketError error);

private slots:
    void timeout();
    void sendPing();
    void socketDisconnected();
    void socketConnected();
    void handleDeleted();
    void messageReceived(QByteArray message);
 //   void messageReceived(QVariantMap msg);
    void errorSlot(QAbstractSocket::SocketError error);
    #ifndef WEB_ASSEMBLY
    void sslErrors(const QList<QSslError> &errors);
    #endif

public slots:
};

#endif // CONNECTIONHANDLER_H
