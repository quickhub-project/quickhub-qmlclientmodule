/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */



#ifndef LISTMODELBASE_H
#define LISTMODELBASE_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>

template <class ItemType>
class ListModelBase : public QAbstractListModel
{

public:
    explicit ListModelBase(QObject *parent=0);
    virtual ~ListModelBase();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void appendRow(ItemType  item);
    void appendRows(const QList<ItemType>  &items);
    void insertRow(int row, ItemType item);
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
    void replaceData(const QList<ItemType>  &newData);
    void replaceItem(int row, const ItemType item);    
    ItemType takeRow(int row);
    ItemType find(const QString &id) const;
    QModelIndex indexFromItem(const ItemType item) const;
    void clear();
    bool isEmpty() const { return m_dataList.isEmpty(); }
    int findItem(ItemType item);
    ItemType at(int index) const { return m_dataList.at(index); }
    virtual int count() const { return m_dataList.count(); }

protected:
    QList<ItemType > m_dataList;

private slots:
    void itemDataChanged();


};

#include "ListModelBase.cpp"
#endif // LISTMODELBASE_H
