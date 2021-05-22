/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QJSValue>
#include <QQmlPropertyMap>
#include <QWebSocket>
#include <QSettings>

#include "../Shared/VirtualConnection.h"

/*!
    \qmltype Device
    \inqmlmodule QuickHub
    \inherits QQmlPropertyMap
    \instantiates Device
    \brief Allows the declarative implementation of new device types.

    Each instance of this object registers with the QuickHub Server as a separate device.
    This makes it possible to share program functionality with other clients in the form
    of properties, events or RPCs.

    The setup of your device should be done in the Component.onCompleted handler. Here you can
    register the RPCs and the properties your device supports. Dont't forget to call start()
    after you have completely setup your device. The whole interface is designed to work well with
    QML and JavaScript.

    1. Register your RPCs with registerFunction()
    2. Register your _writable_ Properties with registerProperty() or registerPropertyWithInitValue()
    3. Register your readonly properties with setProperty()
    4. Last step: call start() to complete device registration.

    Dont forget: Notify the QuickHub server whenever a property changes. Just call setProperty()

    Example:
      \qml

        Device
        {
            id: device
            uuid: "424711" // must be unique for each INSTANCE!
        }
        Component.onCompleted:
        {
            // setup writable Properties without default value
            device.registerProperty("pricePerUnit", function(data)
            {
                // important: confirm change request!
                device.setProperty("pricePerUnit", data.val)
                docroot.pricePerUnit = data.val
            });

            // setup writable Properties with default value
            device.registerPropertyWithInitValue("unitDuration", function(data)
            {
                docroot.unitDuration = data.val

                // important: confirm change request!
                device.setProperty("unitDuration", docroot.unitDuration)
            }, 60000);

            // setup RPCs
            device.registerFunction("logout", logout)

            // setup readonly properties
            device.setProperty("currentUser", ...);

            // start device - don't forget!
            device.start()
        }

        function logout()
        {
           //do logout
        }
      \endqml

*/

class Device : public QQmlPropertyMap
{
    Q_OBJECT

    /*!
        \qmlproperty QString Device::type
        Specifies the type of the device
    */
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

    /*!
        \qmlproperty QString Device::uuid
        Specifies the uniquie ID of the device.
        \note A client can implement several different devices at once.
        It is important that each device gets a unique ID. This ID is only valid
        for this one instance of the Device. It must be uniform and unique and must
        not change from the moment the Device is registered with a mapping.
    */
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)

    /*!
        \qmlproperty QString Device::uuid
        Specifies the shortID of the device. The short ID should be a string that is
        very easy to remember. The idea behind is to make the registration process more user-friendly.
        \note The short ID may change in contrast to the uuid. Nevertheless it should be unique.
        A randomly generated string of 4 letters and numbers has worked well in practice.
    */
    Q_PROPERTY(QString shortID READ shortID WRITE setShortID NOTIFY shortIDChanged)

    /*!
        \qmlproperty bool Device::initialized
        Becomes true when the Device has sent its parameters to the QuickHub server.
        \note This has nothing to do with the server side device registration (Assignment of an internal QuickHub device address). It only says that the
        device object is connected to the server.
    */
    Q_PROPERTY(bool initialized READ initialized  NOTIFY initializedChanged)

    /*!
        \qmlproperty bool Device::connected
        Becomes true when the Device has received its init parameters. (Parameters that contain property change requests while the device was offline).
        If this is true, the device is successfully connected to the server.
        \note This has nothing to do with the server side device registration (Assignment of an internal QuickHub device address). It only says that the
        device object is connected to the server.
    */
    Q_PROPERTY(bool connected READ connected  NOTIFY connectedChanged)



public:
    Device(QObject* parent = nullptr);
    ~Device();

    /*!
      \fn void Device::registerFunction(QString name,  QJSValue callback);
      Registers a function (RPC) with the given name.
      The callback javascript function will be called with the apropriate parameters.
      Use this function to perform single actions onto the device.
    */
    Q_INVOKABLE void registerFunction(QString name,  QJSValue callback);

    /*!
      \fn void Device::registerProperty(QString name,  QJSValue callback, QVariant value = QVariant())
      Registers a new writable property with the given name. Readonly properties can be initialized via setProperty().
      The callback is called whenever a client triggers a change request. Never forget to confirm the property
      change request by calling setProperty() with a value. If the requested set value is invalid for any
      reason (out of range or wrong format or something else), a value must still be confirmed. This can also be the
      last valid value. It's part of the protocol to confirm a value whenever a change request is triggered! (see the
      example in the class description)

      If the value already exists and can be read at the time of registration, it can be initialized immediately.
      If no value is given, the property is initialized with the last confirmed value.
      The device class persists all properties, so that they are preserved even after a restart.

      \note A property change request that is not confirmed remains in the dirty state.
      This should be avoided under all circumstances!
      \sa void Device::registerPropertyWithInitValue(QString name,  QJSValue callback, QVariant init)
    */

    Q_INVOKABLE void registerProperty(QString name,  QJSValue callback, QVariant value = QVariant());

    /*!
        \fn void Device::registerPropertyWithInitValue(QString name,  QJSValue callback, QVariant init)
        Compared to the value of registerProperty() the value of registerPropertyWithInitValue() is only
        used to initialize a default value. This value represents the initial state and is only relevant
        for the first and initial registration of the device property. Besides this, it behaves exactly like
        Device::registerProperty(QString name,  QJSValue callback, QVariant value)
        \note You should use either registerProperty() or registerPropertyWithInitValue() for the same property.
        Using both doesn't make sense and will lead in undefined beahaviour. Don't forget to call start() after
        the setup of your device.
        \sa void Device::registerProperty(QString name,  QJSValue callback, QVariant value)
    */
    Q_INVOKABLE void registerPropertyWithInitValue(QString name,  QJSValue callback, QVariant init);

    /*!
        \fn void Device::setProperty(QString name, QVariant value);
        Notifies the server that a property has changed. This function can also be used to initialize readonly
        properties. (See example in class description). This function has to be called WHENEVER a property changes.
        Especially in property change request handler functions. It's part of the protocol to confirm a value
        whenever a change request is triggered!
    */
    Q_INVOKABLE void setProperty(QString name, QVariant value);

    /*!
        \fn void Device::sendMessage(QString subject, QVariantMap data);
        Sends a message with generic data. This message can received by every client who is listening to the devices
        event channel. Use this function only to communicate atomic events.  State changes should preferably be
        represented by properties.
        \note Messages are not stored by the server. They expire when no one is listening.
    */
    Q_INVOKABLE void sendMessage(QString subject, QVariantMap data);

    /*!
        \fn void Device::start();
        Call this function after you have completed all calls to register properties and RPCs. See the class description
        for an example.
    */
    Q_INVOKABLE void start();

    QString uuid() const;
    void setUuid(const QString &uuid);

    QString type() const;
    void setType(const QString &type);

    QString shortID() const;
    void setShortID(const QString &shortID);

    bool initialized() const;
    bool connected() const;

protected:
    QVariant updateValue(const QString &key, const QVariant &input);

private:
    // the cache stores property changes while device is still initializing
    QVariantMap             _tempCache;
    QSettings               _settings;
    bool                    _connected = false;
    VirtualConnection*      _conn;
    QMap<QString, QJSValue> _functions;
    QString                 _uuid;
    QString                 _shortID;
    QString                 _type;
    bool                    _initialized = false;
    bool                    _readyForInit = false;
    void initDevice();
    void propertyDataChanged(const QString &key, const QVariant &input);

signals:
    void uuidChanged();
    void initializedChanged();
    void typeChanged();
    void propertyUpdate(QString name, QVariant value);
    void shortIDChanged();
    void connectedChanged();

private slots:
    void connectToServer();
    void connectedSlot();
    void disconnectedSlot();

    void messageReceived(QVariant message);


};

#endif // DEVICE_H
