/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "CloudModel.h"
#include <QDebug>
#include "ConnectionManager.h"
#include <QUuid>
#include <QJsonDocument>
#include <QJSEngine>
#include <QCoreApplication>
#include "Helpers/QHSettings.h"
#include <QWebSocket>


CloudModel* CloudModel::_instance = nullptr;

CloudModel::CloudModel(QObject *parent) : QObject(parent)
{
    _connectionManager = ConnectionManager::instance();
    _connection = _connectionManager->getConnection();
    _vconnection = _connectionManager->getVConnection();
    if(QHSettings::instance()->ready())
        loadSettings();
    else
        connect(QHSettings::instance(), &QHSettings::readyChanged, this, &CloudModel::loadSettings);
    connect(_vconnection, &VirtualConnection::messageReceived, this, &CloudModel::messageReceived);
    connect(_vconnection, &VirtualConnection::connected, this, &CloudModel::socketConnected);
}

void CloudModel::loadSettings()
{
    QHSettings* settings = QHSettings::instance();
    int size = settings->beginReadArray("logins");
    for (int i = 0; i < size; ++i)
    {
        settings->setArrayIndex(i);
        QVariantMap login;
        login["userName"] = settings->value("userName").toString();
        login["password"] = settings->value("password").toString();
        _lastLogins [settings->value("server").toString()] = login;
    }
    settings->endArray();
    _autoLogin = settings->value("autoLogin", false).toBool();
     Q_EMIT autoLoginChanged();
}

void CloudModel::addLogin(const QVariantMap &login)
{
    QString server = _connectionManager->getServer();
    _lastLogins.insert(server, login);
    QHSettings* settings = QHSettings::instance();
    settings->beginWriteArray("logins");
    QMapIterator<QString, QVariantMap> it(_lastLogins);
    int i = 0;
    while(it.hasNext())
    {
        it.next();
        settings->setArrayIndex(i++);
        settings->setValue("userName", it.value()["userName"].toString());
        settings->setValue("password", it.value()["password"].toString());
        settings->setValue("server", it.key());
    }
    settings->endArray();
}

bool CloudModel::autoLogin() const
{
    return _autoLogin;
}

void CloudModel::setAutoLogin(bool newAutoLogin)
{
    if (_autoLogin == newAutoLogin)
        return;

    _autoLogin = newAutoLogin;
    QHSettings::instance()->setValue("autoLogin", _autoLogin);

    emit autoLoginChanged();
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

void CloudModel::login(QString user, QString password, QJSValue callback, bool rememberLogin)
{
    _userID = user;
    _password = password;

    if(rememberLogin)
    {
        QVariantMap login;
        login["userName"] = user;
        login["password"] = password;
        addLogin(login);
    }

    if(_connectionManager->getState() == ConnectionManager::STATE_Connected)
    {
        QVariantMap msg;
        msg["command"] = "user:login";
        QVariantMap payload;
        payload["password"] = password;
        payload["userID"] = user;
        msg["payload"] = payload;
        _connectionManager->setConnectionState(ConnectionManager::STATE_Authenticating);
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
    if(_connectionManager->getState() >= ConnectionManager::STATE_Connected)
    {
        QVariantMap msg;
        msg["command"] = "user:add";
        msg["token"] = _connectionManager->getToken();
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
    if(_connectionManager->getState() >= ConnectionManager::STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:changepassword";
        msg["token"] = _connectionManager->getToken();;
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
    if(_connectionManager->getState() >= ConnectionManager::STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:setpermission";
        msg["token"] = _connectionManager->getToken();;
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
    if(_connectionManager->getState() >= ConnectionManager::STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:delete";
        msg["token"] = _connectionManager->getToken();;
        QVariantMap payload;
        payload["userID"] = userID;
        msg["payload"] = payload;
        _deleteUserCb = callback;
        _vconnection->sendVariant(msg);
    }
}

void CloudModel::deleteCurrentUser(QString password, QJSValue callback)
{
    if(_connectionManager->getState() >= ConnectionManager::STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:delete";
        msg["token"] = _connectionManager->getToken();;
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
    if(_connectionManager->getState() == ConnectionManager::STATE_Authenticated)
    {
        QVariantMap msg;
        msg["command"] = "user:logout";
        msg["token"] = _connectionManager->getToken();;
        _vconnection->sendVariant(msg);
    }
}

void CloudModel::setTemporaryAutoLogin()
{
    _tempAutoLogin = true;
}

void CloudModel::init()
{
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

void CloudModel::socketConnected()
{
    QString server = _connectionManager->getServer();

    if(!_user.isEmpty() && !_password.isEmpty() && _tempAutoLogin)
    {
        login(_userID, _password);
        _tempAutoLogin = false;
        return;
    }

    if( _autoLogin && _lastLogins.contains(server))
    {
        login(_lastLogins[server]["userName"].toString(), _lastLogins[server]["password"].toString());
        return;
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
        _connectionManager->setToken(payload["token"].toString());
        _connectionManager->setConnectionState(ConnectionManager::STATE_Authenticated);
        _user = payload["user"].toMap();
        Q_EMIT currentUserChanged();
        _loginCb.call(QJSValueList { true, 0 });
        return;
    }

    if(command == "user:login:failed")
    {
        _connectionManager->setConnectionState(ConnectionManager::STATE_Connected);
        int errorCode =  answer["errrorcode"].toInt();
        _errorString =  answer["errorstring"].toString();
        Q_EMIT onErrorStringChanged();
        _loginCb.call(QJSValueList { false , errorCode});
        return;
    }

    if(command == "logout:success")
    {
         _connectionManager->setConnectionState(ConnectionManager::STATE_Connected);
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
        _setPermissionCb.call(QJSValueList { false , errorCode});
        return;
    }
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


