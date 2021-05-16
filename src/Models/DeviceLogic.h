/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICELOGIC_H
#define DEVICELOGIC_H

#include "../Shared/VirtualConnection.h"
#include <QObject>
#include <QSettings>
#include <DeviceLogicProperty.h>
#include <QSharedPointer>

typedef QSharedPointer<DeviceLogicProperty> devicePropertyPtr;
class DeviceLogic : public QObject
{
    Q_OBJECT

public:
    explicit DeviceLogic(QObject *parent = nullptr);
    void registerProperty(devicePropertyPtr property);
    void sendMessage(QString subject, QVariantMap data = QVariantMap());
    void start();

    QString uuid() const;
    void setUuid(const QString &uuid);

    QString shortID() const;
    void setShortID(const QString &shortID);

    QString getType() const;
    void setType(const QString &type);

    bool getInitialized() const;
    bool getConnected() const;

public slots:
    void setProperty(QString name, QVariant value);

private:
    void                                initDevice();
    QString                             getSetterName(QString propertyName);
    QVariantMap                         _tempCache;
    QMap<QString, devicePropertyPtr>    _properties;
    bool                                _connected = false;
    VirtualConnection*                  _conn = nullptr;
    QString                             _uuid;
    QString                             _shortID;
    bool                                _initialized = false;
    bool                                _readyForInit = false;
    QSettings                           _settings;
    QStringList                         _functions;
    QString                             _type;


signals:
    void uuidChanged();
    void initializedChanged();
    void typeChanged();
    void propertyUpdate(QString name, QVariant value);
    void shortIDChanged();
    void connectedChanged();

    void functionCalled(QString functioName, QVariantMap args);


private slots:
    void connectToServer();
    void connectedSlot();
    void disconnectedSlot();
    void messageReceived(QVariant message);

signals:

};

#endif // DEVICELOGIC_H
