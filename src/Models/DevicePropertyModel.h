/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICEPROPERTYMODEL_H
#define DEVICEPROPERTYMODEL_H

#include <QObject>
#include <QVariant>

#include "DeviceModel.h"

/*!
    \qmltype DevicePropertyModel
    \inqmlmodule CloudModels
    \inherits QObject
    \brief This class allows access to all aspects of a device property.

    This class allows access to all aspects of a device property.
    Instances can be obtained via DeviceModel::getProperty()
*/

class DevicePropertyModel : public QObject
{
    friend class DeviceModel;
    Q_OBJECT

    /*!
      \qmlproperty QVariant DevicePropertyModel::value
      Holds the value of the property. If the property is dirty, the set-value will be returned.
    */
    Q_PROPERTY(QVariant value READ getValue WRITE sendValue NOTIFY valueChanged)

    /*!
      \qmlproperty QVariant DevicePropertyModel::setValue
      Holds the set-value of the property. No matter if the set-value was confirmed or not.
    */
    Q_PROPERTY(QVariant setValue READ getSetValue NOTIFY setValueChanged)

    /*!
      \qmlproperty QVariant DevicePropertyModel::realValue
      Holds the real-value of the property. No matter if there is a newer set-value or not.
    */
    Q_PROPERTY(QVariant realValue READ getRealValue NOTIFY realValueChanged)

    /*!
      \qmlproperty QString DevicePropertyModel::name
      Holds the name of the property
    */
    Q_PROPERTY(QString name READ getName NOTIFY nameChanged)

    /*!
      \qmlproperty QString DevicePropertyModel::description
      Holds the optional description (display-name) of the property.
      \note This is stored in the metadata of the property. The extra property is only to simplify bindings.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

    /*!
       \qmlproperty QString DevicePropertyModel::description
       Holds the optional unitString of the property.
       \note This is stored in the metadata of the property. The extra property is only to simplify bindings.
    */
    Q_PROPERTY(QString unitString READ unitString WRITE setUnitString NOTIFY unitStringChanged)

    // well,I do not believe that this property has ever been used.
    // It just returns false... I mark it as deprecated for now.
    Q_PROPERTY(bool confirmed READ getConfirmed NOTIFY confirmed)

    /*!
       \qmlproperty qlonglong DevicePropertyModel::confirmedTimestamp
       This property holds the timestamp when the last property change reqeust was confirmed by the device.
       The returned value are msecs since epoch.
    */
    Q_PROPERTY(qlonglong confirmedTimestamp READ confirmedTimestamp NOTIFY confirmedTimestampChanged)

    /*!
       \qmlproperty qlonglong DevicePropertyModel::iconId
       Holds an optional string which can be used to map to an icon
       \note This is stored in the metadata of the property.The extra property is only to simplify bindings.
    */
    Q_PROPERTY(QString iconId READ iconId NOTIFY iconIdChanged)

    /*!
       \qmlproperty bool DevicePropertyModel::initialized
       Is true, when the device property model is completely initialized.
    */
    Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)

    /*!
       \qmlproperty bool DevicePropertyModel::dirty
       Is true, whenever a property change request is newer the the last confirmed value.
       This is the case when the device was offline or the device hasn't received the change
        request for other reasons.
    */
    Q_PROPERTY(bool dirty READ isDirty NOTIFY dirtyChanged)

    /*!
       \qmlproperty bool DevicePropertyModel::editable
       This property indicates whether the property is writable or only readable.
    */
    Q_PROPERTY(bool editable READ getEditable NOTIFY editableChanged)

public:
    QVariant    getValue() const;
    QVariant    getRealValue() const;
    QVariant    getSetValue() const;
    QString     getName() const;


    void        sendValue(QVariant value);

    bool        initialized() const;
    void        emitValueChanged();

    QString     unitString() const;
    void        setUnitString(const QString &unitString);

    QString     description() const;
    void        setDescription(const QString &description);

    bool        isDirty() const;
    qlonglong   confirmedTimestamp() const;

    QString     iconId() const;
    void        setIconId(const QString &iconId);

    bool        getConfirmed();
    QVariantMap toMap() const;

    bool        getEditable() const;
    void        setEditable(bool editable);

signals:
    void realValueChanged(QString name, QVariant realValue);
    void setValueChanged(QString name, QVariant setValue);
    void dirtyChanged(QString name, bool isDirty);
    void confirmed(bool accepted);
    void unitStringChanged(QString name, QString unit);
    void confirmedTimestampChanged(QString name, qlonglong timestamp);
    void iconIdChanged(QString name, QString iconId);
    void descriptionChanged(QString name, QString description);
    void nameChanged();
    void initializedChanged();
    void editableChanged();
    void valueChanged();


    void metadataEdited(QString name, QString key, QVariant value);

    // this signal will be emmited, when the "value" property gets modified via QML
    void sendValueToDevice(QString name, QVariant property);

private:
    void setMedatada(QString key, QVariant value);
    void setRealValue(const QVariant &getRealValue);
    void setSetValue(const QVariant &setValue);
    void setDirty(bool isDirty);
    void init(QVariantMap initData);
    explicit DevicePropertyModel(QString name, DeviceModel* parent, QVariantMap metadata = QVariantMap());
    ~DevicePropertyModel();

private:
    QString       _name;
    bool          _editable = false;
 //   DeviceHandle* _handle;
    QVariantMap   _metadata;
    QVariant      _realValue;
    QVariant      _setValue;
    QString       _unitString;
    QString       _description;
    bool          _dirty;
    bool          _initialized = false;
    qlonglong     _confirmedTimestamp;
    QString       _iconId;

};

#endif // DEVICEPROPERTYMODEL_H
