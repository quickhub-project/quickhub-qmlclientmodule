/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#ifndef CLOUDMODEL_H
#define CLOUDMODEL_H

#include <QObject>
#include <QJSValue>
#include "../Shared/Connection.h"
#include "../Shared/VirtualConnection.h"

/*!
    \qmltype QuickHub
    \inqmlmodule QuickHub
    \inherits QObject
    \instantiates CloudModel
    \brief Encapsulates all functions around connection, login and user administration
*/

class QQmlEngine;
class QJSEngine;
class ConnectionManager;
class CloudModel : public QObject
{
    Q_OBJECT

    /*!
        \qmlproperty QVariantMap CloudModel::currentUser
        Holds detailed informations about the current user.
    */
    Q_PROPERTY(QVariantMap currentUser READ getCurrentUser NOTIFY currentUserChanged)

    /*!
        \qmlproperty bool CloudModel::autoLogIn
        If this property is true, the client remembers the login credentials (user and password) for a successful login.
        The next time the client tries to connect to the same server, a login with these credentials will be tried.
        \default true
    */
    Q_PROPERTY(bool autoLogIn MEMBER _autoLogin NOTIFY autoLoginChanged)

    /*!
        \qmlproperty QString ConnectionState::errorString
        In the error state an error message can be retrieved via this property.
    */
    Q_PROPERTY(QString errorString READ getErrorString NOTIFY onErrorStringChanged)


public:
    /*!
        \enum CloudModel::ErrorCodes
        This enum specifies the state of the connection.

        \value NoError
            No errors
        \value UserAlreadyExists
            The user you are trying to create already exists
        \value IncompleteData
            Not all parameters necessary for the action were given.
        \value InvalidData
            The specified parameters are invalid
        \value PermissionDenied
            The currently logged in user does not have the necessary authorization
        \value IncorrectPassword
            The given password is not valid
        \value UserNotExists
            Will be returned if the corresponding user does not exist.
    */

    enum ErrorCodes
    {
        NoError = 0,
        UserAlreadyExists = -1,
        IncompleteData = - 2,
        InvalidData = -3,
        PermissionDenied = -4,
        IncorrectPassword = -5,
        UserNotExists = -6,
    };
    Q_ENUM(ErrorCodes)

    static CloudModel* instance();
    static QObject* instanceAsQObject(QQmlEngine *engine = nullptr, QJSEngine *scriptEngine = nullptr);


    /*!
        \fn void CloudModel::login(QString user, QString password, QJSValue callback = QJSValue(), bool remember = false)
        tries to log the user in with the given credentials. This function can only be called if the connection state is STATE_Connected.
        If remember is set to true, the client stores the credentials internally on the device so that they are used when autologin = true.

        the callback is called with two parameters. The first is success (boolean) and the second is the
        errorcode, in case success == false.
        \sa CloudModel::ErrorCodes
    */
    Q_INVOKABLE void login(QString user, QString password, QJSValue callback = QJSValue(), bool remember = false);


    /*!
        \fn void CloudModel::setupTunnel(QString server, QJSValue callback = QJSValue())
        This function is an alpha feature and is currently not officially supported. This function connects to a QuickHub instance
        via a separate gateway. This is to allow connections to instances that are located inside protected networks. For example
        behind a router.
    */
    Q_INVOKABLE void setupTunnel(QString server, QJSValue callback = QJSValue());

    /*!
        \fn void CloudModel::addUser(QString userID, QString password, QJSValue callback = QJSValue())
        Will add a new user with the given parameters.

        The callback is called with two parameters. The first is success (boolean) and the second is the
        errorcode, in case success == false.
        \sa CloudModel::ErrorCodes
    */

    Q_INVOKABLE void addUser(QString userID, QString password, QJSValue callback = QJSValue());

    /*!
        \fn void CloudModel::addUser(QString userID, QString password, QString eMail, QJSValue callback = QJSValue())
        Will add a new user with the given parameters.

        The callback is called with two parameters. The first is success (boolean) and the second is the
        errorcode, in case success == false.
        \sa CloudModel::ErrorCodes
    */
    Q_INVOKABLE void addUser(QString userID, QString password, QString eMail, QJSValue callback = QJSValue());

    /*!
        \fn void CloudModel::addUser(QString userID, QString password, QString eMail, QString name, QJSValue callback = QJSValue())
        Will add a new user with the given parameters.

        This function can only be called by users with admin privileges.
        Therefore the necessary key "isAdmin" must be true.

        The callback is called with two parameters. The first is success (boolean) and the second is the
        errorcode, in case success == false.
        \sa CloudModel::ErrorCodes
    */
    Q_INVOKABLE void addUser(QString userID, QString password, QString eMail, QString name, QJSValue callback = QJSValue());

    /*!
        \fn void CloudModel::changePassword(QString oldPassword, QString newPassword,QJSValue callback = QJSValue())
        Changes the password for the currently logged in user

        The callback is called with two parameters. The first is success (boolean) and the second is the
        errorcode, in case success == false.
        \sa CloudModel::ErrorCodes
    */
    Q_INVOKABLE void changePassword(QString oldPassword, QString newPassword,QJSValue callback = QJSValue());

    /*!
        \fn void CloudModel::setPermission(QString userID, QString permission, bool allowed, QJSValue callback = QJSValue())
        Adds or changes permissions for a specific user. Permissions consist of keys in the form of strings,
        whose permissions can be either true or false. An unstored key has the same meaning as the key with the
        value false.

        The meaning of certain keys depends on the concrete implementation in the plugins. Consult the plugin specification to see what
        keys exist and what their meaning is.

        This function can only be called by users with admin privileges.
        Therefore the necessary key "isAdmin" must be true.

        The callback is called with two parameters. The first is success (boolean) and the second is the
        errorcode, in case success == false.
        \sa CloudModel::ErrorCodes
    */
    Q_INVOKABLE void setPermission(QString userID, QString permission, bool allowed, QJSValue callback = QJSValue());

    /*!
        \fn void CloudModel::deleteUser(QString userID, QJSValue callback = QJSValue())
        Deletes another user from the system. This action cannot be undone!

        This function can only be called by users with admin privileges.
        Therefore the necessary key "isAdmin" must be true.

        The callback is called with two parameters. The first is success (boolean) and the second is the
        errorcode, in case success == false.
        \sa CloudModel::ErrorCodes
    */
    Q_INVOKABLE void deleteUser(QString userID, QJSValue callback = QJSValue());

    /*!
        \fn void CloudModel::deleteCurrentUser(QString password, QJSValue callback = QJSValue())
        Deletes the currently logged in user.

        The callback is called with two parameters. The first is success (boolean) and the second is the
        errorcode, in case success == false.
        \sa CloudModel::ErrorCodes
    */
    Q_INVOKABLE void deleteCurrentUser(QString password, QJSValue callback = QJSValue());

    /*!
        \fn void CloudModel::logout()
        logs out the currently logged in user. The connection to the server remains established.
        After successful logout the client should be in the state "STATE_Connected".
    */
    Q_INVOKABLE void logout();


//  Q_INVOKABLE void connectToHub(QString server, QJSValue callback);

    bool loggedIn() const;
    void setLoggedIn(bool loggedIn);

    QString getErrorString() const;
    QString getUserID() const;
    QVariantMap getCurrentUser() const;

private:
    explicit CloudModel(QObject *parent = nullptr);
    void loadLogins();
    void addLogin(const QVariantMap& login);
    Connection*         _connection;
    ConnectionManager*   _connectionManager;
    static CloudModel*  _instance;
    QVariantMap         _user;
    VirtualConnection*  _vconnection;
    QString             _errorString;
    QString             _userID;
    QJSValue            _addUserCb;
    QJSValue            _setupTunnelCb;
    QJSValue            _loginCb;
    QJSValue            _changePwCb;
    QJSValue            _setPermissionCb;
    QJSValue            _deleteUserCb;
    bool                _autoLogin = false;
    QMap<QString, QVariantMap>  _lastLogins;

signals:
    void connectedChanged();
    void tokenChanged();
    void onErrorStringChanged();
    void currentUserChanged();
    void autoLoginChanged();

private slots:
    void socketConnected();
    void messageReceived(const QVariant& data);


public slots:
};

#endif // CLOUDMODEL_H
