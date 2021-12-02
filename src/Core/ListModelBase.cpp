/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "ListModelBase.h"
#include <QtDebug>

template <class ItemType>
ListModelBase<ItemType>::ListModelBase(QObject *parent)
    : QAbstractListModel(parent)
{ }

template <class ItemType>
int ListModelBase<ItemType>::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_dataList.size();
}


template <class ItemType>
ListModelBase<ItemType>::~ListModelBase()
{
    clear();
}

template <class ItemType>
void ListModelBase<ItemType>::appendRow(ItemType item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_dataList.append(item);
    endInsertRows();
}

template <class ItemType>
void ListModelBase<ItemType>::appendRows(const QList<ItemType> &items)
{
    if (items.isEmpty())
        return;

    beginInsertRows(QModelIndex(),rowCount(),rowCount()+items.size()-1);
    for(auto item : items)
    {
        m_dataList.append(item);
    }
    endInsertRows();
}

template <class ItemType>
void ListModelBase<ItemType>::insertRow(int row, ItemType item)
{
    beginInsertRows(QModelIndex(),row,row);
    m_dataList.insert(row,item);
    endInsertRows();
}

template <class ItemType>
ItemType ListModelBase<ItemType>::find(const QString &id) const
{
    for(auto item : m_dataList)
        if(item->getId() == id) return item;
    return 0;
}

template <class ItemType>
QModelIndex ListModelBase<ItemType>::indexFromItem(const ItemType item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_dataList.size();++row)
        if(m_dataList.at(row) == item) return index(row);

    return QModelIndex();
}

template <class ItemType>
void ListModelBase<ItemType>::clear()
{
    if (m_dataList.isEmpty()) return;
    beginRemoveRows(QModelIndex(),0, m_dataList.size()-1);
    while(!m_dataList.isEmpty()) {
        auto i = m_dataList.takeFirst();
    }
    endRemoveRows();
}

template <class ItemType>
int ListModelBase<ItemType>::findItem(ItemType item)
{
    return m_dataList.indexOf(item);
}

template <class ItemType>
bool ListModelBase<ItemType>::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (count == 0)
        return true;

    if(row < 0 || (row+count) > m_dataList.size())
        return false;

    beginRemoveRows(QModelIndex(),row,row+count-1);
    for(int i=0; i<count; ++i)
    {

       auto j = m_dataList.takeAt(row);
    }
    endRemoveRows();
    return true;
}

template <class ItemType>
ItemType ListModelBase<ItemType>::takeRow(int row)
{
    beginRemoveRows(QModelIndex(),row,row);
    auto item = m_dataList.takeAt(row);
    endRemoveRows();
    return item;
}

template <class ItemType>
void ListModelBase<ItemType>::replaceData(const QList<ItemType> &newData)
{
    clear();
    appendRows(newData);
}

template <class ItemType>
void ListModelBase<ItemType>::replaceItem(int row, const ItemType item)
{
  m_dataList.replace(row, item);
  Q_EMIT dataChanged(index(row),index(row));
}

template <class ItemType>
void ListModelBase<ItemType>::itemDataChanged()
{
    auto sndr = dynamic_cast<ItemType>(sender());
    if(sndr)
    {
        int idx = m_dataList.indexOf(sndr);
        if(idx > -1 && idx < count())
        {
            auto modelIdx = index(idx);
            emit dataChanged(modelIdx, modelIdx);
        }
    }
}

