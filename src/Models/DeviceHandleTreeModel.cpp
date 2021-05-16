/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include "DeviceHandleTreeModel.h"
#include "QJsonDocument"

DeviceHandleTreeModel::DeviceHandleTreeModel(QObject *parent) : QAbstractItemModel(parent),
     _communicationHandler(new ResourceCommunicationHandler("list", this))
{
    connect(_communicationHandler, &ResourceCommunicationHandler::newMessage, this, &DeviceHandleTreeModel::messageHandler);
    connect(_communicationHandler, &ResourceCommunicationHandler::attachedChanged, this, &DeviceHandleTreeModel::attachedChanged);
    _communicationHandler->setDescriptor("deviceHandles");


    rootItem = new TreeItem("root");

    /*
    TreeItem* childA = new TreeItem("A");
    TreeItem* childB = new TreeItem("B");
    TreeItem* childA1 = new TreeItem("A1");
    TreeItem* childA2 = new TreeItem("A2");
    */

}

QVariant DeviceHandleTreeModel::getData(QModelIndex index, QString role)
{
    return data(index, roleNames().key(role.toLocal8Bit()));
}

QVariant DeviceHandleTreeModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    /*
    if (role != Qt::DisplayRole)
    {
        qDebug()<<"WTF";

    }*/

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if(role == Qt::DisplayRole)
        return item->path();

    if(role == Qt::UserRole+1)
        return item->isDevice();

    if(role == Qt::UserRole+2)
        return item->deviceMapping();

    return QVariant();

}

QHash<int, QByteArray> DeviceHandleTreeModel::roleNames() const
{

    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::UserRole+1, "isDevice");
    roles.insert(Qt::UserRole+2, "mapping");
    /*
    if(_listData.count() > 0)
    {
        QVariantMap map = _listData.at(0).toMap();
        for(int i = 0; i < map.keys().size(); i++)
        {
            roles.insert(i, map.keys().at(i).toLatin1());
        }
    }*/

    return roles;
}

int DeviceHandleTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
  if (parent.column() > 0)
      return 0;

  if (!parent.isValid())
      parentItem = rootItem;
  else
      parentItem = static_cast<TreeItem*>(parent.internalPointer());

  return parentItem->childCount();
}

QModelIndex DeviceHandleTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
             return QModelIndex();

         TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
         TreeItem *parentItem = childItem->parentItem();

         if (parentItem == rootItem)
             return QModelIndex();

         return createIndex(parentItem->row(), 0, parentItem);
}

int DeviceHandleTreeModel::columnCount(const QModelIndex &parent) const
{

    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QModelIndex DeviceHandleTreeModel::index(int row, int column, const QModelIndex &parent) const
{

    if (!hasIndex(row, column, parent))
           return QModelIndex();

       TreeItem *parentItem;

       if (!parent.isValid())
           parentItem = rootItem;
       else
           parentItem = static_cast<TreeItem*>(parent.internalPointer());

       TreeItem *childItem = parentItem->child(row);
       if (childItem)
           return createIndex(row, column, childItem);
       else
           return QModelIndex();
}

bool DeviceHandleTreeModel::sendMessage(QVariantMap msg)
{
    return _communicationHandler->sendMessage(msg);
}

void DeviceHandleTreeModel::messageHandler(QVariant message)
{
    QVariantMap msg = message.toMap();
    QVariantMap parameters = msg["parameters"].toMap();
    QString cmd = msg["command"].toString();

    qDebug()<<cmd;
    if(cmd == "list:dump")
    {
        QVariantList list = parameters["data"].toList();
        _listData = list;
        setupModelData();
        return;
    }

    int idx = parameters["index"].toInt();
    QVariant data = parameters["data"];

    if(cmd == "list:property:set")
    {
        QString property = parameters["property"].toString();
        if(idx >= _listData.count())
        {
            return;
        }
        QVariantMap item = _listData.at(idx).toMap();
        item[property] = data;
        _listData.replace(idx, item);
        return;
    }

    if(cmd == "list:set")
    {
        _listData.replace(idx, data);
      //  setupModelData();
        return;
    }

    if(cmd == "list:insertat")
    {

        _listData.insert(idx, data);
        setupModelData();
        return;
    }

    if(cmd == "list:remove")
    {
        _listData.removeAt(idx);
        setupModelData();
        return;
    }
}

void DeviceHandleTreeModel::attachedChanged()
{
    if(!_communicationHandler->isAttached())
    {
   //     beginResetModel();
            _listData.clear();
     //   endResetModel();
    }
}

void DeviceHandleTreeModel::setupModelData()
{
    beginResetModel();

    delete rootItem;
    rootItem = new TreeItem("root");
    for(int i = 0; i < _listData.count(); i++)
    {
        QVariantList mappings = _listData.at(i).toMap()["mappings"].toList();
        foreach (QVariant mapping, mappings)
        {
            addNode(mapping.toString());
        }
    }
    endResetModel();
}

void DeviceHandleTreeModel::addNode(QString path)
{
    QStringList items = path.split("/");
    TreeItem* lastItem = rootItem;

    for(int i = 0; i < items.count(); i++)
    {
        QString subpath = items.at(i);
        TreeItem* child = lastItem->hasChild(subpath);
        if(child)
        {
            lastItem = child;
        }
        else
        {

            child = new TreeItem(subpath);
            if(i == items.count()-1)
            {
                child->setIsDevice(true);
                child->setDeviceMapping(path);
            }
            lastItem->appendChild(child);
            lastItem = child;
        }
    }
}
