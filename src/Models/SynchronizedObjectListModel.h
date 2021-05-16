/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef SYNCHRONIZEDOBJECTLISTMODEL_H
#define SYNCHRONIZEDOBJECTLISTMODEL_H

#include <QObject>
#include <QQmlPropertyMap>
#include "SynchronizedListLogic.h"
#include <QQmlParserStatus>

/*!
    The SynchronizedListLookupModel turns the list entries into a key value
    map, where any property can be specified as key. With the corresponding
    keys the individual list entries can be accessed. The getItem() function
    will return a QQmlProperty map representing the corresponding list entry.
    The QQmlPropertyMap instance can be used for read and write access to the
    list entry.

    Take the following list, contained by ListResource "myData"

    [{"ID":4711"val": "foo"},{"ID":4711, "val": "foo"},{ "ID":4711"val": "bar"}]

    then you can accesss the elements as follows

    SychronizedListLookupModel{
         id: lookupModel
         resource:"myData"
         lookupKey:"ID"
    }

    ListModel{
        model: lookupModel.keys
        delegate:TextField {
            text:lookupModel.getItem(modelData).val
        }
    }

    This model is very useful if you want to display or edit a set of
    items regardless of their index position. Especially if there are
    foreign key relationships to other entities. Then the primary key
    can be used as a lookupKey to quickly get the corresponding item.

    To avoid timing and initialization problems while setting up bindings
    before the models are initialized, the model will also return objects
*/

class SynchronizedObjectListModel : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_INTERFACES(QQmlParserStatus)

    /*!
        \qmlproperty QString SynchronizedObjectListModel::lookupKey
        Specifies which property should be taken as lookup key.
    */
    Q_PROPERTY(QString lookupKey READ getLookupKey WRITE setLookupKey NOTIFY lookupKeyChanged )

    /*!
        \qmlproperty QString SynchronizedObjectListModel::resource
        The resource identifier is used to determine from which resource the data should be loaded.
        The combination of resource and type is unique.
    */
    Q_PROPERTY(QString resource READ getResource WRITE setResource NOTIFY resourceChanged)

    /*!
        \qmlproperty QVariantMap SynchronizedObjectListModel::filter
        If the resource supports data filtering on the server side, the necessary parameters can be specified here.
        Structure and semantics of the data is defined by the plugin which implements the resource.  Consult the
        documentation of the plugin to find out how filtering of data is supported.
    */
    Q_PROPERTY(QVariantMap filter READ getFilter WRITE setFilter NOTIFY filterChanged)

    /*!
        \qmlproperty QStringList SynchronizedObjectListModel::keys
        holds a QStringList with the key values. Which means the values of the properties with the given lookupKey
    */
    Q_PROPERTY(QStringList keys READ keys  NOTIFY keysChanged)

    /*!
        \qmlproperty bool SynchronizedObjectListModel::keepDeletedItems
        This way, bindings can be preserved in case an object with the same key-value is added again during runtime

    */
    Q_PROPERTY(bool keepDeletedItems MEMBER _keepDeletedItems NOTIFY keepdeletedItemsChanged)

    /*!
    \qmlproperty bool SynchronizedObjectListModel::initialized

    */
    Q_PROPERTY(bool initialized READ getInitialized NOTIFY initializedChanged)

    /*!
    \qmlproperty int SynchronizedObjectListModel::count
    Returns the item count.
    */
    Q_PROPERTY(int count READ getCount NOTIFY keysChanged)

public:

    /*!
        \fn QObject* SynchronizedObjectListModel::getItem(QString key)
        Returns a QVariantMap representing the list entry for the given key
        To avoid time and initialization problems, empty objects are returned even
        if there is no object in the list for the corresponding key. The bindings
        are then created later, if a corresponding item has been added to the list.

        \note check the value of the _exists field to see if the item really exists
        in the origin resource data. The field cann also toggle to true, if an appropriate
        item comes afterwards.
    */
    Q_INVOKABLE QObject *getItem(QString key);

    /*!

    */
    Q_INVOKABLE bool contains(QString key);
    Q_INVOKABLE void insert(QObject* obj);
    Q_INVOKABLE void insert(QVariantMap data);
    Q_INVOKABLE bool deleteItem(QString key);

    explicit SynchronizedObjectListModel(QObject *parent = nullptr);
    virtual void componentComplete() override;
    virtual void classBegin() override {}

    QString getLookupKey() const;
    void setLookupKey(const QString& lookupKey);

    QVariantMap getFilter() const;
    void setFilter(const QVariantMap &filter);

    QString getResource() const;
    void setResource(const QString &resourceName);

    QStringList keys() const;
    bool getInitialized();

    int getCount() const;

signals:

private slots:
    void itemsAppended(QVariantList items);
    void mapValueChanged(const QString &key, const QVariant &input);
    void itemPropertyChanged(int index, QString property, QVariant data);
    void itemAdded(int index, QVariant data);
    void itemRemoved(int index);
    void listCleared();

private:
    QQmlPropertyMap*                initPropertyMap(QVariantMap item, QQmlPropertyMap* map = nullptr);
    QVariantMap                     _filter;
    QString                         _lookupKey;
    QMap<QString, QQmlPropertyMap*> _map;
    QMap<QString, QQmlPropertyMap*> _nullItems;
    QStringList                     _keyList;
    SynchronizedListLogic*          _logic = nullptr;
    bool                            _complete = false;
    QString                         _resourceName;
    bool                            _keepDeletedItems = false;



signals:
    void sigItemAdded(QString id, QVariantMap item);
    void sigItemRemoved(QString id);
    void lookupKeyChanged();
    void filterChanged();
    void resourceChanged();
    void keysChanged();
    void keepdeletedItemsChanged();
    void initializedChanged();

};

#endif // SYNCHRONIZEDOBJECTLISTMODEL_H
