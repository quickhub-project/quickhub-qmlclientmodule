/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef SynchronizedListModel2_H
#define SynchronizedListModel2_H

#include "../Core/ListModelBase.h"
#include "../Core/ResourceCommunicationHandler.h"
#include "../Shared/VirtualConnection.h"
#include "SynchronizedListLogic.h"
#include <QObject>
#include <QQmlParserStatus>

/*!
    \qmltype SynchronizedListModel
    \inqmlmodule CloudModels
    \inherits ListModelBase<QVariant>
    \instantiates SynchronizedListModel2
    \brief Provides access to QuickHub list resources

    In addition, this class is the UI model part of SynchronizedListModel. The WebSocket
    interface to the server is wrapped in SynchronizedListLogic
*/

class SynchronizedListModel2 : public ListModelBase<QVariant>, public QQmlParserStatus
{
    Q_OBJECT
    /*!
        \qmlproperty int SynchronizedListModel2::count
        Holds the row count / item count of the list
    */
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    /*!
        \qmlproperty int SynchronizedListModel2::preloadCount
        The number of items that are initially loaded. If the list contains
        more elements, the list is automatically reloaded when scrolling.
        \sa QAbstractItemModel::fetchMore, QAbstractItemModel::canFetchMore
    */
    Q_PROPERTY(int preloadCount READ preloadCount WRITE setPreloadCount NOTIFY preloadCountChanged)

    /*!
        \qmlproperty int SynchronizedListModel2::resource
        The resource identifier is used to determine from which resource the data should be loaded.
        The combination of resource and type is unique.
    */
    Q_PROPERTY(QString resource READ getResource WRITE setResource NOTIFY resourceChanged)

    /*!
        \qmlproperty bool SynchronizedListModel2::connected
        Connected is true if the resource is successfully connected to its server-side counterpart
        By setting this property a reconnect to the corresponding resource is automatically triggered.
        During the reconnect process the initialized property holds the value false.
    */
    Q_PROPERTY(bool connected READ getConnected NOTIFY connectedChanged)

    /*!
        \qmlproperty ResourceCommunicationHandler::ModelState state
        Holds the current state of the resource.
    */
    Q_PROPERTY(ResourceCommunicationHandler::ModelState state READ getModelState NOTIFY modelStateChanged)

    /*!
        \qmlproperty QVariantMap SynchronizedListModel2::metadata
        The metadata can contain any information, such as the purpose, content or destination of the data contained.
    */
    Q_PROPERTY(QVariantMap metadata READ getMetadata NOTIFY metadataChanged)

    /*!
        \qmlproperty QVariantMap SynchronizedListModel2::filter
        If the resource supports data filtering on the server side, the necessary parameters can be specified here.
        Structure and semantics of the data is defined by the plugin which implements the resource.  Consult the
        documentation of the plugin to find out how filtering of data is supported.
    */
    Q_PROPERTY(QVariantMap filter READ getFilter WRITE setFilter NOTIFY filterChanged)

    /*!
        \qmlproperty bool SynchronizedListModel2::initialized
        initialized is true if all data was successfully loaded after the connection was established.
    */
    Q_PROPERTY(bool initialized READ getInitialized NOTIFY initializedChanged)
    Q_INTERFACES(QQmlParserStatus)

public:

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    /*!
        \fn QString SynchronizedListModel2::getUserIDForIndex(int index)
        Returns the user who created ord modified the appropriate object.
        This feature is not supported by all resources.
    */

    Q_INVOKABLE QString getUserIDForIndex(int index);

    /*!
        \fn qint64 SynchronizedListModel2::getTimestampForIndex(int index)
        Returns the time when the corresponding object was created or last edited.
        This feature is not supported by all resources.
    */
    Q_INVOKABLE qint64 getTimestampForIndex(int index);

    /*!
        \fn QString SynchronizedListModel2::getUUIDForIndex(int index)
        Returns the uuid hash for the given index.
    */
    Q_INVOKABLE QString getUUIDForIndex(int index);

    /*!
        \fn void SynchronizedListModel2::insertAt(int index, QVariantMap obj)
        Returns the uuid hash for the given index.
    */
    Q_INVOKABLE void insertAt(int index, QVariantMap obj);

    /*!
        \fn void SynchronizedListModel2::append(QObject* obj)
        Appends the given QObject to the list. The object is serialized by reading the properties
        via the meta object system.
    */
    Q_INVOKABLE void append(QObject* obj);

    /*!
        \fn void SynchronizedListModel2::append(QVariantMap obj)
        Appends the given QVariantMap to the list.
        \note JSON objects are automatically converted to QVariantMaps during the transfer from QML to C++
    */
    Q_INVOKABLE void append(QVariantMap data);

    /*!
        \fn void SynchronizedListModel2::append(QVariantList list)
        Appends the given list of objects to the list. (QList<QString, QVariantMap> -> QVariantList)
        \note JSON objects are automatically converted to QVariantMaps during the transfer from QML to C++
    */
    Q_INVOKABLE void appendList(QVariantList list);

    /*!
        \fn void SynchronizedListModel2::set(int index, QVariantMap data)
        Overwrites / resets the object at the given index.
        \note JSON lists are automatically converted to QVariantLists during the transfer from QML to C++
    */
    Q_INVOKABLE void set(int index, QVariantMap data);

    /*!
        \fn void SynchronizedListModel2::clear()
        Removes and deletes all items from the list. This action cannot be undone.
    */
    Q_INVOKABLE void clear();

    /*!
        \fn void SynchronizedListModel2::remove(int index)
        Removes and deletes all the apropriate item with the given index from list.
        This action cannot be undone.
    */
    Q_INVOKABLE void remove(int index);

    /*!
        \fn void SynchronizedListModel2::deleteList()
        Removes and deletes all the apropriate item with the given index from list.
        This action cannot be undone.
    */
    Q_INVOKABLE void deleteList();

    /*!
        \fn void SynchronizedListModel2::setProperty(int index, QString property, QVariant val)
        Sets the value of the property of an object at a certain index
    */
    Q_INVOKABLE void setProperty(int index, QString property, QVariant val);

    /*!
        \fn QVariant SynchronizedListModel2::get(int index)
        Returns the object at the given index.
    */
    Q_INVOKABLE  QVariant get(int index);

    /*!
        \fn QVariant SynchronizedListModel2::get(int index)
        Returns the current index for the object with the given uuid.
    */
    Q_INVOKABLE  int getIndexForUUID(QString uuid);

    //ctor
    explicit SynchronizedListModel2(QObject *parent = nullptr);

    // QAbstractListModel API
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    virtual void componentComplete() override;
    virtual void classBegin() override {}
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;
    mutable QHash<int, QByteArray> _roles;


    // Property getters & setters
    bool getConnected();

    QVariantMap getMetadata() const;
    Q_INVOKABLE void setMetadata(const QVariantMap &metadata);

    ResourceCommunicationHandler::ModelState getModelState() const;

    QString getResource() const;
    void setResource(const QString &resourceName);

    QVariantMap getFilter() const;
    void setFilter(const QVariantMap &filter);

    int preloadCount() const;
    void setPreloadCount(int preloadCount);

    bool getInitialized();

signals:
    void sigItemAdded(int index, QVariant item);
    void sigItemRemoved(int index);
    void countChanged();
    void resourceChanged();
    void connectedChanged();
    void modelStateChanged();
    void metadataChanged();
    void listSuccessfullModified();
    void filterChanged();
    void preloadCountChanged();
    void listModified();
    void initializedChanged();

private:
    SynchronizedListLogic*  _logic;
    QVariantMap             _filter;
    int                     _preloadCount=50;
    bool                    _complete = false;
    QString                 _resourceName;

public slots:
    void disconnectList();
    void connectList();

    void itemPropertyChanged(int index, QString property, QVariant data);
    void itemUpdated(int index, QVariant data);
    void itemAdded(int index, QVariant data);
    void itemRemoved(int index);
    void listCleared();
    void itemsAppended(QVariantList items);
};

#endif // SynchronizedListModel2_H
