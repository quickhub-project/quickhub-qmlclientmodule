/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "quickhubclient_plugin.h"


#include <QNetworkInterface>
#include <QDebug>
#include "src/InitCloudModels.h"

void QuickHubClientPlugin::registerTypes(const char *uri)
{
    // @uri CloudModels
    InitCloudModels::registerTypes(uri);
}

