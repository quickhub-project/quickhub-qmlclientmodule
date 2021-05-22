/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "CloudModel.h"
#include <QDebug>

#include <QUuid>
#include <QJsonDocument>
#include <QJSEngine>
#include <QCoreApplication>
#include <QSettings>
#include <QWebSocket>

CloudModel* CloudModel::_instance = nullptr;

CloudModel::CloudModel(QObject *parent) : QObject(parent),
    _token("")
{
    loadLogins();
    _connection = new Connection(this);
    _vconnection = new VirtualConnection(_connection);

    connect(_vconnection, &VirtualConnection::messageReceived, this, &CloudModel::messageReceived);
    connect(_vconnection, &VirtualConnection::connected, this, &CloudModel::socketConnected);
    connect(_vconnection, &VirtualConnection::disconnected, this, &CloudModel::socketDisconnected);
    connect(_connection, &Connection::socketError, this, &CloudModel::socketError);
}

void CloudModel::loadLogins()
{
    QSettings settings;
    int size = settings.beginReadArray("logins");
    for (int i = 0; i < size; ++i)
    {
        qDebug()<<settings.value("server").toString();
        settings.setArrayIndex(i);
        QVariantMap login;
        login["userName"] = settings.value("userName").toString();
        login["password"] = settings.value("password").toString();
        _lastLogins [settings.value("server").toString()] = login;
    }
    settings.endArray();
}

void CloudModel::addLogin(const QVariantMap &login)
{
    qDebug()<<"SAVE";
    _lastLogins.insert(_server, login);
    QSettings settings;
    settings.beginWriteArray("logins");
    QMapIterator<QString, QVariantMap> it(_lastLogins);
    int i = 0;
    while(it.hasNext())
    {
        it.next();
        settings.setArrayIndex(i++);
        settings.setValue("userName", it.value()["userName"].toString());
        settings.setValue("password", it.value()["password"].toString());
        settings.setValue("server", it.key());
    }
    settings.endArray();
    settings.setValue("lastLogin", _server);
}

CloudModel *CloudModel::instance()
{
    if(!_instance)
        _instance = new CloudModel(QCoreApplication::instance());

    return _instance;
}

QObject *CloudModel::instanceAsQObject(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    Q_UNUSED(engine)
    return instance();
}

void CloudModel::login(QString user, QString password, QJSValue callback, bool remember)
{
    _userID = user;

    if(remember)
    {
        QVariantMap login;
        login["userName"] = user;
        login["password"] = password;
        addLogin(login);
    }

    if(_connectionState == STATE_Connected)
    {
        QVariantMap msg;
        msg["command"] = "user:login";
        QVariantMap payload;
        payload["password"] = password;
        payload["userID"] = user;
        msg["payload"] = payload;
        _connectionState = STATE_Authenticating;
        Q_EMIT onStateChanged();
        _vconnection->sendVariant(msg);
        _loginCb = callback;
    }
}

void CloudModel::addUser(QString userID, QString password, QJSValue callback)
{
    addUser(userID,  password,  "", "", callback);
}


void CloudModel::addUser(QString userID, QString password, QString eMail,  QJSValue callback)
{
    addUser(userID,  password,  eMail, "", callback);
}


void CloudModel::addUser(QString userID, QString password, QString eMail, QString name, QJSValue callback)
{
    if(_connectionState >= STATE_Connected)
    {
        QVariantMap msg;
        msg["command"] = "user:add";
        msg["token"] = _token;
        QVariantMap payload;
        payload["password"] = password;
        payload["userID"] = userID;
        if(!eMail.isEmpty())
            payload["eMail"] = eMail;
        if(!name.isEmpty())
            payload["name"] = name;
        msg["payload"] = payload;
        _addUserCb = callback;
        _vconnection->sendVariant(msg);
    }
}

void CloudModel::changePassword(QString oldPassword, QString newPassword, QJSValue callback)
{
    if(_connectionState >= STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:changepassword";
        msg["token"] = _token;
        QVariantMap payload;
        payload["oldPassword"] = oldPassword;
        payload["newPassword"] = newPassword;
        msg["payload"] = payload;
        _changePwCb = callback;
        _vconnection->sendVariant(msg);
    }
}

void CloudModel::setPermission(QString userID, QString permission, bool allowed, QJSValue callback)
{
    if(_connectionState >= STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:setpermission";
        msg["token"] = _token;
        QVariantMap payload;
        payload["userID"] = userID;
        payload["permission"] = permission;
        payload["allowed"] = allowed;
        msg["payload"] = payload;
        _setPermissionCb = callback;
        _vconnection->sendVariant(msg);
    }
}

void CloudModel::deleteUser(QString userID, QJSValue callback)
{
    if(_connectionState >= STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:delete";
        msg["token"] = _token;
        QVariantMap payload;
        payload["userID"] = userID;
        msg["payload"] = payload;
        _deleteUserCb = callback;
        _vconnection->sendVariant(msg);
    }
}

void CloudModel::deleteCurrentUser(QString password, QJSValue callback)
{
    if(_connectionState >= STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:delete";
        msg["token"] = _token;
        QVariantMap payload;
        payload["userID"] = _userID;
        payload["password"] = password;
        msg["payload"] = payload;
        _deleteUserCb = callback;
        _vconnection->sendVariant(msg);
    }
}

void CloudModel::logout()
{
    if(_connectionState == STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:logout";
        msg["token"] = _token;
        _vconnection->sendVariant(msg);
    }
}


void CloudModel::connectToServer(QString server, QJSValue callback)
{
    if(!_connection->getSocket())
        _connection->setSocket(new QWebSocket());

    _connectCb = callback;
    if(_connection->isConnected())
        _connection->disconnect();

    _connectionState = STATE_Connecting;
    Q_EMIT onStateChanged();
    _server = server;
    _connection->connect(server);
    Q_EMIT onServerUrlChanged();

}


void CloudModel::setupTunnel(QString server, QJSValue callback)
{
    _setupTunnelCb = callback;
    QVariantMap msg;
    msg["command"] = "quickhub:connect";
    QVariantMap parameters;
    parameters["name"] = server;
    msg["parameters"] = parameters;
    _vconnection->sendVariant(msg);
}

void CloudModel::disconnectServer()
{
    _connection->disconnect();
}

void CloudModel::reconnectServer()
{
    if(!_connection->isConnected())
    {
        _connection->connect(_server);
    }
}

Connection *CloudModel::getConnection()
{
    return _connection;
}

void CloudModel::socketConnected()
{
    _connectionState = STATE_Connected;
    Q_EMIT onStateChanged();
    qDebug()<<_lastLogins.count();

    if( _autoLogin && _lastLogins.contains(_server))
    {
        login(_lastLogins[_server]["userName"].toString(), _lastLogins[_server]["password"].toString());
    }
}

void CloudModel::socketDisconnected()
{
    _connectionState = STATE_Disconnected;
    Q_EMIT onStateChanged();
}

void CloudModel::socketError(QAbstractSocket::SocketError error)
{
    if(_connectCb.isCallable())
        _connectCb.call(QJSValueList { false, error });

    if(error == QAbstractSocket::NetworkError)
    {
        _connection->connect(_server);
    }
}

void CloudModel::messageReceived(const QVariant& data)
{
    QVariantMap answer = data.toMap();
    QVariantMap payload = answer["payload"].toMap();
    QString command = answer["command"].toString();


    if(command == "quickhub:connect:success")
    {
        _connection->reset();
    }

    if(command == "user:add:success")
    {
        _addUserCb.call(QJSValueList { true, 0 });
        return;
    }

    if(command == "user:add:failed")
    {
        _errorString =  answer["errorstring"].toString();
        int errorCode =  answer["errrorcode"].toInt();
        Q_EMIT onErrorStringChanged();
        _addUserCb.call(QJSValueList { false, errorCode });
        return;
    }

    if(command == "user:login:success")
    {
        _connectionState = STATE_Authenticated;
        _token = payload["token"].toString();
        _user = payload["user"].toMap();
        Q_EMIT currentUserChanged();
        Q_EMIT onStateChanged();
        Q_EMIT tokenChanged();
        _loginCb.call(QJSValueList { true, 0 });
        return;
    }

    if(command == "user:login:failed")
    {
        _connectionState = STATE_Connected;
        Q_EMIT onStateChanged();
        int errorCode =  answer["errrorcode"].toInt();
        _errorString =  answer["errorstring"].toString();
        Q_EMIT onErrorStringChanged();
        _loginCb.call(QJSValueList { false , errorCode});
        return;
    }

    if(command == "logout:success")
    {
        _connectionState = STATE_Connected;
        Q_EMIT onStateChanged();
        return;
    }

    if(command == "user:changepassword:success")
    {
        _changePwCb.call(QJSValueList { true , 0});
        return;
    }

    if(command == "user:changepassword:failed")
    {
        int errorCode =  answer["errrorcode"].toInt();
        _errorString =  answer["errorstring"].toString();
        Q_EMIT onErrorStringChanged();
        _changePwCb.call(QJSValueList { false , errorCode});
        return;
    }

    if(command == "user:delete:success")
    {
        _deleteUserCb.call(QJSValueList { true , 0});
        return;
    }

    if(command == "user:delete:failed")
    {
        int errorCode =  answer["errrorcode"].toInt();
        _errorString =  answer["errorstring"].toString();
        Q_EMIT onErrorStringChanged();
        _deleteUserCb.call(QJSValueList { false , errorCode});
        return;
    }


    if(command == "user:setpermission:success")
    {
        _setPermissionCb.call(QJSValueList { true , 0});
        return;
    }

    if(command == "user:setpermission:failed")
    {
        int errorCode =  answer["errrorcode"].toInt();
        // _errorString =  answer["errorstring"].toString();
       // Q_EMIT onErrorStringChanged(); TODO
        _setPermissionCb.call(QJSValueList { false , errorCode});
        return;
    }
}

QString CloudModel::getServer() const
{
    return _server;
}

void CloudModel::setServer(const QString &server)
{
    if(_server == server)
        return;

    connectToServer(server);
}

QString CloudModel::getToken() const
{
    return _token;
}

CloudModel::ConnectionState CloudModel::getState() const
{
    return _connectionState;
}

void CloudModel::setConnectionState(const ConnectionState &connectionState)
{
    _connectionState = connectionState;
}

QString CloudModel::getErrorString() const
{
    return _errorString;
}

QString CloudModel::getUserID() const
{
    return _userID;
}

QVariantMap CloudModel::getCurrentUser() const
{
    return _user;
}

int CloudModel::getKeepaliveInterval()
{
    return _keepaliveInterval;
}

void CloudModel::setKeepaliveInterval(int interval)
{
    _connection->setKeepAlive(interval);
    Q_EMIT keepaliveIntervalChanged();
}
