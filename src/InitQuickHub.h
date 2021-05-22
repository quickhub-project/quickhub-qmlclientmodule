/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef INITQuickHub_H
#define INITQuickHub_H

#include "SynchronizedListModel2.h"
#include "SynchronizedObjectModel.h"
#include "UserListModel.h"
#include "AutomationRule.h"
#include "RoleFilter.h"
#include "CloudModel.h"
#include "ResourceCommunicationHandler.h"
#include "DeviceListModel.h"
#include "DeviceHandleListModel.h"
#include "DeviceModel.h"
#include "Device.h"
#include "DevicePropertyModel.h"
#include "DeviceHandleTreeModel.h"
#include "ImageCollectionModel.h"
#include "ServiceModel.h"
#include "SynchronizedObjectListModel.h"
#include "FilteredDeviceModel.h"
//#include "FileUploader.h"
#include <qqml.h>

class InitQuickHub
{

public:
    static void registerTypes(const char *uri)
    {
        qmlRegisterUncreatableType<ResourceCommunicationHandler>(uri, 1, 0, "ModelState", "only to access enumTypes");
        qmlRegisterUncreatableType<DevicePropertyModel>(uri, 1, 0, "DevicePropertyModel", "only to access enumTypes");
        qmlRegisterType<SynchronizedListModel2>(uri, 1, 0, "SynchronizedListModel");
        qmlRegisterType<SynchronizedObjectModel>(uri, 1, 0, "SynchronizedObjectModel");
        qmlRegisterType<ServiceModel>(uri, 1, 0, "ServiceModel");
        qmlRegisterType<ImageCollectionModel>(uri, 1, 0, "ImageCollectionModel");
        qmlRegisterType<UserListModel>(uri, 1, 0, "UserListModel");
        qmlRegisterType<RoleFilter>(uri, 1, 0, "RoleFilter");
        qmlRegisterType<AutomationRule>(uri, 1, 0, "AutomationRule");
        qmlRegisterType<DeviceListModel>(uri, 1, 0, "DeviceListModel");
        qmlRegisterType<DeviceHandleListModel>(uri, 1, 0, "DeviceHandleListModel");
        qmlRegisterType<DeviceHandleTreeModel>(uri, 1, 0, "DeviceHandleTreeModel");
        qmlRegisterType<FilteredDeviceModel>(uri, 1, 0, "FilteredDeviceModel");
        qmlRegisterType<DeviceModel>(uri, 1, 0, "DeviceModel");
        qmlRegisterType<Device>(uri, 1, 0, "Device");
        qmlRegisterSingletonType<CloudModel>(uri, 1, 0, "QuickHub", &CloudModel::instanceAsQObject);
        qmlRegisterType<SynchronizedObjectListModel>(uri, 1, 0, "SynchronizedListLookupModel");
//        qmlRegisterType<FileUploader>(uri, 1, 0, "FileUploader");
    }
};

#endif // INITQuickHub_H
