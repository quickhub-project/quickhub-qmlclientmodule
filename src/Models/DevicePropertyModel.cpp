/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "DevicePropertyModel.h"

DevicePropertyModel::DevicePropertyModel(QString name, DeviceModel *parent, QVariantMap initData) :QObject(parent),
    _name(name)
{
    _name  = name;
    Q_EMIT nameChanged();
    if(!initData.isEmpty())
        init(initData);
}

QVariant DevicePropertyModel::getValue() const
{
    return _dirty ? _setValue : _realValue;
}

QVariant DevicePropertyModel::getRealValue() const
{
    return _realValue;
}

QVariant DevicePropertyModel::getSetValue() const
{
    return _setValue;
}

QString DevicePropertyModel::getName() const
{
    return _name;
}

void DevicePropertyModel::sendValue(QVariant value)
{
    Q_EMIT sendValueToDevice(_name, value);
}

bool DevicePropertyModel::initialized() const
{
    return _initialized;
}

void DevicePropertyModel::emitValueChanged()
{
    Q_EMIT valueChanged();
}

QString DevicePropertyModel::unitString() const
{
    return _unitString;
}

void DevicePropertyModel::setUnitString(const QString &unitString)
{
    qDebug()<<"META EDIT B";
    Q_EMIT metadataEdited(_name, "unit", unitString);
}

QString DevicePropertyModel::description() const
{
    return _description;
}

void DevicePropertyModel::setDescription(const QString &description)
{
   Q_EMIT metadataEdited(_name, "desc", description);
}

bool DevicePropertyModel::isDirty() const
{
    return _dirty;
}

qlonglong DevicePropertyModel::confirmedTimestamp() const
{
    return 0;
}

QString DevicePropertyModel::iconId() const
{
    return _iconId;
}

void DevicePropertyModel::setIconId(const QString &iconId)
{
    Q_EMIT metadataEdited(_name, "icon", iconId);
}

bool DevicePropertyModel::getConfirmed()
{
    return false;
}

QVariantMap DevicePropertyModel::toMap() const
{
    return QVariantMap();
}

void DevicePropertyModel::setMedatada(QString key, QVariant value)
{
    if(key == "desc")
    {
        _description = value.toString();
        Q_EMIT descriptionChanged(_name, _description);
    }

    if(key == "icon")
    {
        _iconId = value.toString();
        Q_EMIT iconIdChanged(_name, _iconId);
    }

    if(key == "unit")
    {
        _unitString = value.toString();
        Q_EMIT unitStringChanged(_name, _unitString);
    }
}

void DevicePropertyModel::setRealValue(const QVariant &realValue)
{
    if(_realValue == realValue)
        return;

    _realValue = realValue;
    Q_EMIT realValueChanged(_name, realValue);
}

void DevicePropertyModel::setSetValue(const QVariant &setValue)
{
    if(_setValue == setValue)
        return;

    _setValue = setValue;
    Q_EMIT setValueChanged(_name, setValue);
}

void DevicePropertyModel::setDirty(bool isDirty)
{
    if(_dirty == isDirty)
        return;

    _dirty = isDirty;
    Q_EMIT dirtyChanged(_name, isDirty);
}

void DevicePropertyModel::init(QVariantMap initData)
{
    _realValue              = initData["val"];
    _setValue               = initData["setVal"];
    _confirmedTimestamp     = initData["confTS"].toLongLong();
    _dirty                  = initData["dirty"].toBool();
    _metadata               = initData["metadata"].toMap();

    _unitString             = _metadata["unit"].toString();
    _description            = _metadata["desc"].toString();
    _iconId                 = _metadata["icon"].toString();

    Q_EMIT confirmedTimestampChanged(_name,_confirmedTimestamp);
    Q_EMIT setValueChanged(_name, _setValue);
    Q_EMIT realValueChanged(_name, _realValue);
    Q_EMIT unitStringChanged(_name, _unitString);
    Q_EMIT descriptionChanged(_name, _description);
    Q_EMIT iconIdChanged(_name, _iconId);
    Q_EMIT dirtyChanged(_name, _dirty);
    Q_EMIT initializedChanged();
    Q_EMIT valueChanged();
    _initialized = true;
}


DevicePropertyModel::~DevicePropertyModel()
{

}

bool DevicePropertyModel::getEditable() const
{
    return _editable;
}

void DevicePropertyModel::setEditable(bool editable)
{
    if(_editable == editable)
        return;

    _editable = editable;
    Q_EMIT editableChanged();
}
