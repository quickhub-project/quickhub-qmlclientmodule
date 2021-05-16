/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "DeviceLogicProperty.h"

DeviceLogicProperty::DeviceLogicProperty(QObject *parent): QObject(parent)
{

}

DeviceLogicProperty::DeviceLogicProperty(QString name, PropertyType type, QObject *parent) : QObject(parent),
    _type(type)
{
    setPropertyName(name);
}

QVariant DeviceLogicProperty::defaultValue() const
{
    return _defaultValue;
}

void DeviceLogicProperty::setDefaultValue(const QVariant &defaultValue)
{
    _defaultValue = defaultValue;
    Q_EMIT defaultValueChanged();
}

QVariant DeviceLogicProperty::getValue() const
{
    return _value;
}

void DeviceLogicProperty::setValue(const QVariant &value)
{
    _value = value;
    Q_EMIT valueChanged(_propertyName, value);
}

void DeviceLogicProperty::requestValueChange(QVariant value)
{
    _requestedSetValue = value;
    Q_EMIT valueRequested(value);
}

QString DeviceLogicProperty::getName() const
{
    return _propertyName;
}

void DeviceLogicProperty::setPropertyName(const QString &propertyName)
{
    _propertyName = propertyName;
}

void DeviceLogicProperty::initialize(QVariant value)
{
    if(!value.isValid())
        requestValueChange(_defaultValue);
    else
        requestValueChange(value);

    _initialized = true;
    Q_EMIT initializedChanged();
}

DeviceLogicProperty::PropertyType DeviceLogicProperty::getType() const
{
    return _type;
}

void DeviceLogicProperty::setType(const PropertyType &type)
{
    _type = type;
}
