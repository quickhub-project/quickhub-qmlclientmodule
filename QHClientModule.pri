# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
# It is part of the QuickHub framework - www.quickhub.org
# Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de

QT += websockets widgets

SOURCES += \
    $$PWD/src/Core/CloudModel.cpp \
    $$PWD/src/Core/ConnectionManager.cpp \
    $$PWD/src/Core/ListModelBase.cpp \
#    $$PWD/src/Helpers/FileUploader.cpp \
    $$PWD/src/Core/StandaloneDevice.cpp \
    $$PWD/src/Helpers/QHSettings.cpp \
    $$PWD/src/Helpers/RoleFilter.cpp \
    $$PWD/src/Models/DeviceLogic.cpp \
    $$PWD/src/Models/DeviceLogicProperty.cpp \
    $$PWD/src/Models/SynchronizedListModel.cpp \
    $$PWD/src/Models/SynchronizedObjectModel.cpp \
    $$PWD/src/Shared/Connection.cpp \
    $$PWD/src/Shared/VirtualConnection.cpp \
    $$PWD/src/Core/ResourceCommunicationHandler.cpp \
    $$PWD/src/Core/BaseCommunicationHandler.cpp \
    $$PWD/src/Models/AbstractListModel.cpp \
    $$PWD/src/Models/DeviceListModel.cpp \
    $$PWD/src/Models/UserListModel.cpp \
    $$PWD/src/Models/DeviceModel.cpp \
    $$PWD/src/Models/DeviceHandleListModel.cpp \
    $$PWD/src/Models/SynchronizedListLogic.cpp \
    $$PWD/src/Models/SynchronizedListModel2.cpp \
    $$PWD/src/Models/Device.cpp \
    $$PWD/src/Models/DevicePropertyModel.cpp \
    $$PWD/src/Helpers/AutomationRule.cpp \
    $$PWD/src/Models/DeviceHandleTreeModel.cpp \
    $$PWD/src/Models/TreeItem.cpp \
    $$PWD/src/Models/ImageCollectionModel.cpp \
    $$PWD/src/Models/ServiceModel.cpp \
    $$PWD/src/Models/SynchronizedObjectListModel.cpp \
    $$PWD/src/Models/DeviceAdapterModel.cpp \
    $$PWD/src/Models/FilteredDeviceModel.cpp


HEADERS += \
    $$PWD/src/Core/CloudModel.h \
    $$PWD/src/Core/ConnectionManager.h \
    $$PWD/src/Core/ListModelBase.h \
 #   $$PWD/src/Helpers/FileUploader.h \
    $$PWD/src/Core/StandaloneDevice.h \
    $$PWD/src/Helpers/QHSettings.h \
    $$PWD/src/Helpers/RoleFilter.h \
    $$PWD/src/InitQuickHub.h \
    $$PWD/src/Models/DeviceLogic.h \
    $$PWD/src/Models/DeviceLogicProperty.h \
    $$PWD/src/Models/SynchronizedListModel.h \
    $$PWD/src/Models/SynchronizedObjectModel.h \
    $$PWD/src/Shared/Connection.h \
    $$PWD/src/Shared/VirtualConnection.h \
    $$PWD/src/Core/ResourceCommunicationHandler.h \
    $$PWD/src/Core/BaseCommunicationHandler.h \
    $$PWD/src/Models/AbstractListModel.h \
    $$PWD/src/Models/DeviceListModel.h \
    $$PWD/src/Models/UserListModel.h \
    $$PWD/src/Models/DeviceModel.h \
    $$PWD/src/Models/DeviceHandleListModel.h \
    $$PWD/src/Models/SynchronizedListLogic.h \
    $$PWD/src/Models/SynchronizedListModel2.h \
    $$PWD/src/Models/Device.h \
    $$PWD/src/Models/DevicePropertyModel.h \
    $$PWD/src/Helpers/AutomationRule.h \
    $$PWD/src/Models/DeviceHandleTreeModel.h \
    $$PWD/src/Models/TreeItem.h \
    $$PWD/src/Models/ImageCollectionModel.h \
    $$PWD/src/Models/ServiceModel.h \
    $$PWD/src/Models/SynchronizedObjectListModel.h \
    $$PWD/src/Models/DeviceAdapterModel.h \
    $$PWD/src/Models/FilteredDeviceModel.h

INCLUDEPATH +=  $$PWD/src/Models \
                $$PWD/src/Core \
                $$PWD/src/Helpers \
                $$PWD/src

RESOURCES +=


