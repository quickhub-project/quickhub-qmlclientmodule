/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef DEVICELISTMODEL_H
#define DEVICELISTMODEL_H

#include <QObject>
#include "AbstractListModel.h"

class DeviceListModel : public AbstractListModel
{
    Q_OBJECT

public:
    DeviceListModel(QObject* parent = nullptr);
    Q_INVOKABLE void setMapping(QString mapping, QString uuid);
    QVariant data(const QModelIndex &index, int role) const override;

protected:
    void messageReceived(QVariant msg) override;
};

#endif // DEVICELISTMODEL_H
