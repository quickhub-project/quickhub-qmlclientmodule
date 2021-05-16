/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "UserListModel.h"

UserListModel::UserListModel(QObject* parent) : AbstractListModel(parent)
{
    setDescriptor("users");
}

void UserListModel::messageReceived(QVariant msg)
{
    Q_UNUSED(msg)
}
