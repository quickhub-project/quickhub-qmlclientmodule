/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QObject>
#include <QQmlPropertyMap>
#include "../Shared/VirtualConnection.h"
#include "../Core/ResourceCommunicationHandler.h"


/*!
    \qmltype DevicePropertyModel
    \inqmlmodule CloudModels
    \inherits QQmlPropertyMap
    \instantiates DevicePropertyModel
    \brief Provides access to QuickHub devices

    This class gives you access to a registered QuickHub device.
    You need to assign a QuickHub address to a registered device.
    As soon as initialized becomes true, bindings to the individual
    properties via the QQmlePropertyMap can be created.

    It is also possible to connect a DeviceModel to an invalid adress upfront and
    hook the device to this address afterwards. You can use getProperty()
    to create bindings to properties before the DeviceModel is attached or
    even before the address points to a valid device mapping.

    This allows you to use DeviceModel in a very declarative manner. Without
    the need of thinking too much about setup time and initialization.

    \sa DevicePropertyModel
*/

class DevicePropertyModel;
class DeviceModel : public QQmlPropertyMap
{
    Q_OBJECT

    /*!
        \qmlproperty QString DeviceModel::resource
        Assign the address to a registered QuickHub device
    */
    Q_PROPERTY(QString resource READ resource WRITE setResource NOTIFY resourceChanged)

    /*!
        \qmlproperty bool DeviceModel::connected
        Returns true when the DeviceModel is attached to the serverside DeviceHandle
        \note This property says nothing about whether the actual device is really online and connected to the server!
    */
    Q_PROPERTY(bool connected READ getConnected NOTIFY connectedChanged)

    /*!
        \qmlproperty ResourceCommunicationHandler::ModelState DeviceModel::connectionState
        Holds the current model state.
    */
    Q_PROPERTY(ResourceCommunicationHandler::ModelState connectionState READ getModelState NOTIFY modelStateChanged)

    /*!
        \qmlproperty bool DeviceModel::deviceOnline
        Is true when the actual device is currently online and connected to the QuickHub server.
    */
    Q_PROPERTY(bool deviceOnline READ deviceOnline NOTIFY deviceOnlineChanged)

    /*!
        \qmlproperty QVariantList DeviceModel::functions
        Holds a list with all RPCs the device provides
    */
    Q_PROPERTY(QVariantList functions READ functions NOTIFY functionsChanged)

    /*!
        \qmlproperty QString DeviceModel::description
        Holds the "human readable" display name of this device.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

    /*!
        \qmlproperty QList<QObject*> DeviceModel::properties
        Holds a list with all DevicePropertyModel instances.
        \sa DevicePropertyModel
    */
    Q_PROPERTY(QList<QObject*> properties READ properties NOTIFY propertiesChanged)

    /*!
        \qmlproperty QString DeviceModel::uuid
        Holds the unique id of the connected device.
    */
    Q_PROPERTY(QString uuid READ uuid NOTIFY uuidChanged)

    /*!
        \qmlproperty QString DeviceModel::uuid
        Holds the device type.
    */
    Q_PROPERTY(QString type READ getType  NOTIFY typeChanged)

    /*!
        \qmlproperty bool DeviceModel::available
        Is true when the attached device address exists. This can also change during runtime.
        This can change at runtime, if the mapping to the corresponding address is done
        afterwards. It is possible to connect a DeviceModel to an adress and to connect it to
        a device afterwards.
    */
    Q_PROPERTY(bool available READ getAvailable  NOTIFY tempChanged)
    Q_PROPERTY(bool initialized READ getInitialized  NOTIFY initializedChanged)
    Q_PROPERTY(QString shortID READ getShortID NOTIFY shortIDChanged)

public:
    explicit DeviceModel(QObject *parent = nullptr);
    Q_INVOKABLE bool triggerFunction(QString name, QVariantMap parameters);
    Q_INVOKABLE DevicePropertyModel* getProperty(QString name);
    Q_INVOKABLE bool hasProperty(QString name);
    Q_INVOKABLE void setJSProperty(QString name, QJSValue val);
    QString resource() const;
    void setResource(const QString &resource);

    QList<QObject *> properties() const;
    QMap<QString, DevicePropertyModel*> deviceProperties() const;

    bool getConnected() const;
    ResourceCommunicationHandler::ModelState getModelState() const;

    bool deviceOnline() const;
    QVariantList functions();

    QString description() const;
    void setDescription(const QString &description);

    QString uuid() const;

    QString getType() const;
    void setType(const QString &type);

    bool getAvailable() const;
    QString getShortID() const;

    bool getInitialized() const;

private:
    void checkIfPropertiesAreEditable();
    DevicePropertyModel* createPropertyModel(QString name, QVariantMap metadata = QVariantMap());

    QMap<QString, DevicePropertyModel*>  _properties;
    ResourceCommunicationHandler*        _communicationHandler;
    QString                              _resource;
    QString                              _description;
    QVariantList                         _functions;
    QString                              _uuid;
    QString                              _suid;
    QString                              _type;
    bool                                 _online = false;
    bool                                 _temporary = true;
    bool                                 _initialized = false;

protected:
    QVariant updateValue(const QString &key, const QVariant &input) override;

private slots:
    void messageReceived(QVariant message);
    void sendVariant(QString property, QVariant value);
    void metadataEdited(QString name, QString key, QVariant value);

signals:
     void dataReceived(QString subject, QVariantMap data);
     void deviceOnlineChanged();
     void resourceChanged();
     void connectedChanged();
     void modelStateChanged();
     void functionsChanged();
     void propertiesChanged();
     void descriptionChanged();
     void typeChanged();
     void uuidChanged();
     void tempChanged();
     void shortIDChanged();
     void initializedChanged();

public slots:
     void connectObject();
     void disconnectObject();

};

#endif // DEVICEMODEL_H
