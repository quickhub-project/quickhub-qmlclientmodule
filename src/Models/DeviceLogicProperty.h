/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICELOGICPROPERTY_H
#define DEVICELOGICPROPERTY_H

#include <QObject>
#include <QVariant>

/*!
    \class DeviceLogicProperty
    \brief This class implements a property object for a virtual device
    \ingroup Devices

    This is the class for properties that belong to the DeviceLogic
    class. DeviceLogic offers the possibility to provide own (virtual)
    devices via C++.

    \note  Difference to DeviceModel: DeviceModel is the receiver
    object to communicate with a remote device. This class would be a
    possible counterpart

    \sa DeviceLogic
*/


class DeviceLogicProperty : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant getValue READ getValue WRITE setValue NOTIFY valueChanged)


public:
    enum PropertyType
    {
        READ_ONLY,
        READ_WRITE

    };

    explicit DeviceLogicProperty(QObject *parent = nullptr);
    explicit DeviceLogicProperty(QString name, PropertyType type = READ_WRITE, QObject *parent = nullptr);

    QVariant    defaultValue() const;
    void        setDefaultValue(const QVariant &defaultValue);

    QVariant    getValue() const;
    void        setValue(const QVariant &getValue);
    void        requestValueChange(QVariant getValue);

    QString     getName() const;
    void        setPropertyName(const QString &getName);

    void        initialize(QVariant getValue);

    PropertyType getType() const;
    void setType(const PropertyType &type);

private:
    QString _propertyName;
    QVariant _defaultValue;
    QVariant _requestedSetValue;
    QVariant _value;
    bool _initialized = false;
    PropertyType _type = READ_WRITE;


signals:
    void defaultValueChanged();
    void valueChanged(QString name, QVariant getValue);
    void valueRequested(QVariant getValue);
    void initializedChanged();

};

#endif // DEVICELOGICPROPERTY_H
