/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "DeviceListModel.h"


DeviceListModel::DeviceListModel(QObject* parent) : AbstractListModel(parent)
{
    setDescriptor("devices");
}

void DeviceListModel::setMapping(QString mapping, QString uuid)
{
    QVariantMap msg;
    msg["command"] = "mapping:set";
    QVariantMap parameters;
    parameters["mapping"] = mapping;
    parameters["uuid"] = uuid;
    msg["parameters"] = parameters;
    sendMessage(msg);
}

void DeviceListModel::messageReceived(QVariant msg)
{
    Q_UNUSED(msg)
}

QVariant DeviceListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < _listData.size())
    {
        QVariantMap map = _listData.at(index.row()).toMap();
        QString roleKey = roleNames().value(role);
        if(roleKey == "properties")
        {
            QVariantList properties;
            QVariantMap propObjects =  map[roleKey].toMap();

            QMapIterator<QString, QVariant> it(propObjects);
            while(it.hasNext())
            {
                it.next();
                QVariantMap prop;
                prop.insert("name", it.key());
                prop.insert("value", it.value());
                properties << prop;
            }
            return properties;
        }

        return map[roleKey];
    }

    return QVariant();
}
