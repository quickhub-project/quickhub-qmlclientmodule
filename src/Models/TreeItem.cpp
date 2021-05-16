/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "TreeItem.h"

TreeItem::TreeItem(QString path, TreeItem *parent)
{
    m_parentItem = parent;
    m_path = path;
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count() + 1;
}

QVariant TreeItem::data(int column) const
{
    return m_itemData.value(column);
}

void TreeItem::setData(QVariantList data)
{
    m_itemData = data;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

TreeItem *TreeItem::hasChild(QString path)
{
    return _childrenMap.value(path, nullptr);
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
    _childrenMap.insert(item->path(), item);
    item->m_parentItem = this;
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

QString TreeItem::path()
{
    return m_path;
}

bool TreeItem::isDevice() const
{
    return m_isDevice;
}

void TreeItem::setIsDevice(bool isDevice)
{
    m_isDevice = isDevice;
}

QString TreeItem::deviceMapping() const
{
    return m_deviceMapping;
}

void TreeItem::setDeviceMapping(const QString &deviceMapping)
{
    m_deviceMapping = deviceMapping;
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}
