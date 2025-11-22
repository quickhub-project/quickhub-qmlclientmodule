/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#ifndef INITQuickHub_H
#define INITQuickHub_H

#include "Models/SynchronizedListModel2.h"
#include "Models/SynchronizedObjectModel.h"
#include "Models/UserListModel.h"
#include "Helpers/RoleFilter.h"
#include "Core/CloudModel.h"
#include "Core/ResourceCommunicationHandler.h"
#include "Models/DeviceListModel.h"
#include "Models/DeviceHandleListModel.h"
#include "Models/DeviceModel.h"
#include "Models/Device.h"
#include "Models/DevicePropertyModel.h"
#include "Models/DeviceHandleTreeModel.h"
#include "Models/ImageCollectionModel.h"
#include "Models/ServiceModel.h"
#include "Models/SynchronizedObjectListModel.h"
#include "Models/FilteredDeviceModel.h"
#include "Core/StandaloneDevice.h"
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
        qmlRegisterType<DeviceListModel>(uri, 1, 0, "DeviceListModel");
        qmlRegisterType<DeviceHandleListModel>(uri, 1, 0, "DeviceHandleListModel");
        qmlRegisterType<DeviceHandleTreeModel>(uri, 1, 0, "DeviceHandleTreeModel");
        qmlRegisterType<FilteredDeviceModel>(uri, 1, 0, "FilteredDeviceModel");
        qmlRegisterType<DeviceModel>(uri, 1, 0, "DeviceModel");
        qmlRegisterType<Device>(uri, 1, 0, "Device");
        qmlRegisterSingletonType<CloudModel>(uri, 1, 0, "UserLogin", &CloudModel::instanceAsQObject);
        qmlRegisterSingletonType<ConnectionManager>(uri, 1, 0, "Connection", &ConnectionManager::instanceAsQObject);
        qmlRegisterSingletonType<StandaloneDevice>(uri, 1, 0, "StandaloneDevice", &StandaloneDevice::instanceAsQObject);
        qmlRegisterType<SynchronizedObjectListModel>(uri, 1, 0, "SynchronizedListLookupModel");

//        qmlRegisterType<FileUploader>(uri, 1, 0, "FileUploader");
    }
};

#endif // INITQuickHub_H
