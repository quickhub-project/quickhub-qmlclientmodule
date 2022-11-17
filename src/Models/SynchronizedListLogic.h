/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef SynchronizedListLogic_H
#define SynchronizedListLogic_H


/*!
    \class SynchronizedListLogic
    \brief This class implements the WebSocket interface to SynchronizedListResources
    \ingroup Resources

    This class implements the WebSocket interface to SynchronizedListResources.
    It provides a Qt-typical API with signals, slots and modify functions.
    Use this class to implement your own UI models. If you are looking for a ready to use
    ListModel then have a look at SynchronizedListModel2.h (SychronizedListModel.h is deprecated!)
    \sa SynchronizedListModel2
*/


#include "../Core/ListModelBase.h"
#include "../Core/ResourceCommunicationHandler.h"
#include "../Shared/VirtualConnection.h"
#include <QObject>
#include <QTimer>

class SynchronizedListLogic : public QObject
{
    Q_OBJECT

    struct MetaInfo
    {
        QString uuid;
        qint64 lastUpdate;
        QString user;
    };

public:
    explicit SynchronizedListLogic(QObject *parent = nullptr);

    QString getUserIDForIndex(int index);
    qint64 getTimestampForIndex(int index);
    QString getUUIDForIndex(int index);

    /*!
        \fn void SynchronizedListLogic::insertAt(int index, QVariantMap obj)
        Returns the uuid hash for the given index.
    */
    void insertAt(int index, QVariantMap obj);

    /*!
        \fn void SynchronizedListLogic::append(QObject* obj)
        Appends the given QObject to the list. The object is serialized by reading the properties
        via the meta object system.
    */
    void append(QObject* obj);

    /*!
        \fn void SynchronizedListLogic::append(QVariantMap obj)
        Appends the given QVariantMap to the list.
        \note JSON objects are automatically converted to QVariantMaps during the transfer from QML to C++
    */
    void append(QVariantMap data);

    /*!
        \fn void SynchronizedListLogic::append(QVariantList list)
        Appends the given list of objects to the list. (QList<QString, QVariantMap> -> QVariantList)
        \note JSON objects are automatically converted to QVariantMaps during the transfer from QML to C++
    */
    void appendList(QVariantList list);

    /*!
        \fn void SynchronizedListLogic::set(int index, QVariantMap data)
        Overwrites / resets the object at the given index.
        \note JSON lists are automatically converted to QVariantLists during the transfer from QML to C++
    */
    void set(int index, QVariantMap data);

    /*!
        \fn void SynchronizedListLogic::clear()
        Removes and deletes all items from the list. This action cannot be undone.
    */
    void clear();

    /*!
        \fn void SynchronizedListLogic::clear()
        Removes and deletes all the apropriate item with the given index from list.
        This action cannot be undone.
    */
    void remove(int index);

    /*!
        \fn void SynchronizedListLogic::deleteList()
        Removes and deletes all the apropriate item with the given index from list.
        This action cannot be undone.
    */
    void deleteList();

    /*!
        \fn SynchronizedListLogic::setFilter(const QVariantMap &filter)
        If the resource supports data filtering on the server side, the necessary parameters can be specified here.
        Structure and semantics of the data is defined by the plugin which implements the resource.  Consult the
        documentation of the plugin to find out how filtering of data is supported.
    */
    void setFilter(const QVariantMap &filter);

    /*!
        \fn void SynchronizedListLogic::setProperty(int index, QString property, QVariant val)
        Sets the value of the property of an object at a certain index
    */
    void setProperty(int index, QString property, QVariant val);

    /*!
        \fn QVariant SynchronizedListLogic::get(int index)
        Returns the current index for the object with the given uuid.
    */
    int getIndexForUUID(QString uuid);

    /*!
        \fn void SynchronizedListLogic::requestDump()
        Reinitializes the whole modelData
    */
    void requestDump();

    /*!
        \fn bool SynchronizedListLogic::getInitialized();
        Returns true when the model is attached to a resource and has received all data.
        \note Bindings maybe won't work until initialized is true!
    */
    bool getInitialized();

    /*!
        bool SynchronizedListLogic::getConnected();
        Returns true when the model is attached to a resource.
    */
    bool getConnected();

    /*!
        \fn QVariantMap SynchronizedListLogic::getMetadata() const
        The metadata can contain any information, such as the purpose, content or destination of the data contained.
    */
    QVariantMap getMetadata() const;

    /*!
        \fn QVariantMap SynchronizedListLogic::setMetadata(const QVariantMap &metadata)
        The metadata can contain any information, such as the purpose, content or destination of the data contained.
    */
    void setMetadata(const QVariantMap &metadata);

    ResourceCommunicationHandler::ModelState getModelState() const;

    QString getResource() const;
    void setResource(const QString &resourceName);

    // API for lazy loading while scrolling
    void loadItems(int from, int count);
    int getRemoteItemCount() const;
    void setPreloadCount(int preloadCount);
    void fetchMore();

signals:
    void resourceChanged();
    void connectedChanged();
    void modelStateChanged();
    void metadataChanged();
    void listSuccessfullModified();

    void itemPropertyChanged(int index, QString property, QVariant data);
    void itemsAdded(int index, QVariantList data);
    void itemUpdated(int index, QVariant data);
    void itemAdded(int index, QVariant data);
    void itemRemoved(int index);
    void itemsAppended(QVariantList data);
    void listCleared();
    void countChanged(int count);
    void initializedChanged(bool initialized);


private:
    void    insertMulti(QVariantList items, int index = -1);
    void    appendMulti(QVariantList items);
    void    insertItem(QVariant item, int index = -1);
    void    removeItem(int index);
    void    updateProperty(qint64 timestamp, QString property, QVariant value, int index = -1);
    void    clearAll();
    void    updateItem(QVariant item, int index = -1);


    ResourceCommunicationHandler* _communicationHandler;
    int checkAndCorrectIndex(int index, QString uuid);
    QList<QVariant>     _pendingMessages;
    QVariantMap         _metadata;
    QString             _resource;
    QList<MetaInfo>     _metaInfo;
    int                 _preloadCount = -1;
    int                 _remoteItemCount = -1 ;
    bool                _initialized = false;

public slots:
    void disconnectList();
    void connectList();
    void stateChanged();

private slots:
    void messageReceived(QVariant message);


public slots:
};

#endif // SynchronizedListLogic_H
