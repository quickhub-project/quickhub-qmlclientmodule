/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef SYNCHRONIZEDOBJECTMODEL_H
#define SYNCHRONIZEDOBJECTMODEL_H

#include <QObject>
#include <QQmlPropertyMap>

#include "../Shared/VirtualConnection.h"
#include "../Core/ResourceCommunicationHandler.h"

/*!
    \qmltype SynchronizedObjectModel
    \inqmlmodule QuickHub
    \inherits QQmlPropertyMap
    \instantiates SynchronizedObjectModel
    \brief Provides access to QuickHub object resources

    In addition, this class is the QML interface to  SynchronizedObjectModel. The WebSocket
    interface to the server is wrapped by this class
*/

class SynchronizedObjectModel : public QQmlPropertyMap
{
    Q_OBJECT


    Q_PROPERTY(QStringList keyValues READ keys NOTIFY keysChanged)

    /*!
        \qmlproperty int SynchronizedObjectModel::resource
        The resource identifier is used to determine from which resource the data should be loaded.
        The combination of resource and type is unique.
    */
    Q_PROPERTY(QString resource READ resource WRITE setResource NOTIFY resourceChanged)

    /*!
        \qmlproperty bool SynchronizedObjectModel::connected
        Connected is true if the resource is successfully connected to its server-side counterpart
        By setting this property a reconnect to the corresponding resource is automatically triggered.
        During the reconnect process the initialized property holds the value false.
    */
    Q_PROPERTY(bool connected READ getConnected NOTIFY connectedChanged)

    /*!
        \qmlproperty ResourceCommunicationHandler::ModelState SynchronizedObjectModel::connectionState
        Holds the current state of the resource.
    */
    Q_PROPERTY(ResourceCommunicationHandler::ModelState connectionState READ getModelState NOTIFY modelStateChanged)

    /*!
        \qmlproperty QVariantMap SynchronizedObjectModel::metadata
        The metadata can contain any information, such as the purpose, content or destination of the data contained.
    */
    Q_PROPERTY(QVariantMap metadata READ getMetadata NOTIFY metadataChanged)

    /*!
        \qmlproperty bool SynchronizedObjectModel::initialized
        initialized is true if all data was successfully loaded after the connection was established.
        \note Bindings maybe won't work until initialized is true!
    */
    Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)

    /*!
        \qmlproperty QVariantMap SynchronizedObjectModel::filter
        If the resource supports data filtering on the server side, the necessary parameters can be specified here.
        Structure and semantics of the data is defined by the plugin which implements the resource.  Consult the
        documentation of the plugin to find out how filtering of data is supported.
    */
    Q_PROPERTY(QVariantMap filter READ getFilter WRITE setFilter NOTIFY filterChanged)

public:
    enum ObjectModelState
    {
        OBJECTMODEL_ERROR = -1,
        OBJECTMODEL_DISCONNECTED = 0,
        OBJECTMODEL_CONNECTING,
        OBJECTMODEL_CONNECTED,
        OBJECTMODEL_TRANSFERRING,
    };

    Q_ENUM (ObjectModelState)

    //ctor
    explicit SynchronizedObjectModel(QObject *parent = 0);

    //dtor
    ~SynchronizedObjectModel();

    /*!
        \fn void SynchronizedObjectModel::setProperty(QString key, QVariant value)
        Sets the property with the corresponding key to the given value
    */
    Q_INVOKABLE void setProperty(QString key, QVariant value);

    // property getter & setter
    QString resource() const;
    void setResource(const QString &resource);
    QStringList keys();
    bool getConnected() const;
    ResourceCommunicationHandler::ModelState getModelState() const;

    QVariantMap getFilter() const;
    void setFilter(const QVariantMap &filter);

    QVariantMap getMetadata() const;

    bool initialized() const;

    /*!
      \fn QVariant SynchronizedObjectModel::updateValue(const QString &key, const QVariant &input)
      overwrites QQmlPropertyMap::updateValue(). Is called, if properties are changed via qml.
    */

    QVariant updateValue(const QString &key, const QVariant &input);

public slots:
    void connectObject();
    void disconnectObject();

protected:

private:
    QSet<QString>                    _keys;
    ResourceCommunicationHandler*   _communicationHandler;
    QVariantMap                     _metadata;
    QVariantMap                     _objectdata;
    QString                         _resourcePath;
    QString                         _resource;
    bool                            _shared;
    bool                            _initialized = false;
    QVariantMap                     _filter;

signals:
    void keysChanged();
    void resourceChanged();
    void connectedChanged();
    void modelStateChanged();
    void metadataChanged();
    void initializedChanged();
    void filterChanged();

private slots:
    void messageReceived(QVariant message);
    void resetProperties();
};

#endif // SYNCHRONIZEDOBJECTMODEL_H
