#include <QApplication>
#include "ConnectionManager.h"


ConnectionManager* ConnectionManager::_instance = nullptr;

ConnectionManager *ConnectionManager::instance()
{
    if(_instance == nullptr)
        _instance = new ConnectionManager(qApp);

    return _instance;
}

ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent)
{
    _connection = new Connection(this);
    connect(_connection, &Connection::socketError, this, &ConnectionManager::socketError);

    _vconnection = new VirtualConnection(_connection);
    connect(_vconnection, &VirtualConnection::connected, this, [=](){ setConnectionState(ConnectionManager::STATE_Connected);});
    connect(_vconnection,  &VirtualConnection::disconnected, this, [=](){setConnectionState(ConnectionManager::STATE_Disconnected);});
}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::setConnectionState(const State &connectionState)
{
    _connectionState = connectionState;
    Q_EMIT onStateChanged();
}

ConnectionManager::State ConnectionManager::getState() const
{
    return _connectionState;
}


QString ConnectionManager::getToken() const
{
    return _token;
}

void ConnectionManager::setToken(QString token)
{
    _token = token;
    Q_EMIT tokenChanged();
}

QString ConnectionManager::getServer() const
{
    return _server;
}

void ConnectionManager::setServer(const QString &server)
{
    if(_server == server)
        return;

    connectToServer(server);
}

void ConnectionManager::connectToServer(QString server, QJSValue callback)
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

void ConnectionManager::socketError(QAbstractSocket::SocketError error)
{
    if(_connectCb.isCallable())
        _connectCb.call(QJSValueList { false, error });

    if(error == QAbstractSocket::NetworkError)
    {
        _connection->connect(_server);
    }
}

void ConnectionManager::disconnectServer()
{
    _connection->disconnect();
}

void ConnectionManager::reconnectServer()
{
    if(!_connection->isConnected())
    {
        _connection->connect(_server);
    }
}

QObject *ConnectionManager::instanceAsQObject(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    Q_UNUSED(engine)
    return instance();
}

Connection* ConnectionManager::getConnection()
{
    return _connection;
}


int ConnectionManager::getKeepaliveInterval()
{
    return _keepaliveInterval;
}

void ConnectionManager::setKeepaliveInterval(int interval, int timeout)
{
    _connection->setKeepAlive(interval, timeout);
    Q_EMIT keepaliveIntervalChanged();
}

VirtualConnection *ConnectionManager::getVConnection()
{
    return _vconnection;
}
