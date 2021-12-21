#ifndef AUTHENTICATIONSTATE_H
#define AUTHENTICATIONSTATE_H

#include <QObject>
#include <QWebSocket>
#include <QJSValue>
#include "../Shared/Connection.h"
#include "../Shared/VirtualConnection.h"

class QQmlEngine;
class ConnectionManager : public QObject
{
    Q_OBJECT

    /*!
        \qmlproperty QString ConnectionState::serverUrl
        Holds the address of the QuickHub server.  When an address is assigned,
        the connection is started automatically.
    */
    Q_PROPERTY(QString serverUrl READ getServer WRITE setServer NOTIFY onServerUrlChanged)

    /*!
        \qmlproperty  ConnectionState ConnectionState::state
        Holds the state of the current connection.
    */
    Q_PROPERTY(State state READ getState  NOTIFY onStateChanged)

    /*!
        \qmlproperty QString ConnectionState::token
        Holds the session token of the currently logged in user
    */
    Q_PROPERTY(QString token READ getToken NOTIFY tokenChanged)

    /*!
        \qmlproperty int ConnectionState::keepaliveInterval
        Keeps the frequency in milliseconds with which the client checks via a ping whether the connection to
        the server is still active.
    */
    Q_PROPERTY(int keepaliveInterval READ getKeepaliveInterval WRITE setKeepaliveInterval NOTIFY keepaliveIntervalChanged)


public:
    /*!
        \enum ConnectionState::ConnectionState
        This enum specifies the state of the connection.

        \value STATE_Disconnected
            There is no connection to a server.
        \value STATE_Connecting
            The client tries to establich a connection
        \value STATE_Connected
            The client has successfully connected to the server. But
            no user is logged in.
        \value STATE_Authenticating
            The client tries to login with the given user credentials.
        \value STATE_Authenticated
            The client is successfully connected to the server and the login process was successful.
    */
    enum State
    {
        STATE_Disconnected = 0,
        STATE_Connecting = 1,
        STATE_Connected = 2,
        STATE_Authenticating = 3,
        STATE_Authenticated = 4
    };
    Q_ENUM(State)

    Q_INVOKABLE void connectToServer(QString server, QJSValue callback = QJSValue());

    /*!
        \fn void ConnectionState::disconnectServer()
        Terminates the connection to the server. The client should be in the state "STATE_Disconnected"
        after a successful call.
    */
    Q_INVOKABLE void disconnectServer();

    /*!
        \fn void ConnectionState::reconnectServer()
        Attempts to re-establish the last used connection.
    */
    Q_INVOKABLE void reconnectServer();

    State getState() const;
    void setConnectionState(const State &onStateChanged);
    QString getToken() const;
    void setToken(QString token);
    QString getServer() const;
    void setServer(const QString &server);
    Connection* getConnection();
    int getKeepaliveInterval();
    void setKeepaliveInterval(int interval);
    VirtualConnection* getVConnection();

    static QObject* instanceAsQObject(QQmlEngine *engine = nullptr, QJSEngine *scriptEngine = nullptr);
    static ConnectionManager* instance();

private:
    explicit ConnectionManager(QObject *parent = nullptr);
    virtual ~ConnectionManager();
    static ConnectionManager* _instance;
    Connection*             _connection;
    int                     _keepaliveInterval = -1;
    State                   _connectionState = STATE_Disconnected;
    QString                 _server;
    QString                 _token;
    QJSValue                _connectCb;
    VirtualConnection*      _vconnection;

private slots:
    void                    socketError(QAbstractSocket::SocketError error);

signals:
    void onStateChanged();
    void onServerUrlChanged();
    void tokenChanged();
    void keepaliveIntervalChanged();
};

#endif // AUTHENTICATIONSTATE_H
