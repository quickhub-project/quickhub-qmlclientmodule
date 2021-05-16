/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#ifndef USERLISTMODEL_H
#define USERLISTMODEL_H

#include <QObject>
#include "AbstractListModel.h"

/*!
    \qmltype UserListModel
    \inqmlmodule CloudModels
    \inherits AbstractListModel
    \instantiates UserListModel
    \brief provides a QAbstractItemModel with all native QuickHub users

    This class implements a ListModel with all users that are natively
    registered in QuickHub (DefaultAuthenticator). Users , from other plugins
    that implement an IAuthenticator are not considered.

    supported fields:

    userID
    userData (additional generic metadata)
    sessionCount (number of active sessions)
    userName
    userPermissions
    eMail
*/

class UserListModel : public AbstractListModel
{
    Q_OBJECT

public:
    UserListModel(QObject* parent = nullptr);

protected:
    void messageReceived(QVariant msg);
};

#endif // USERLISTMODEL_H
