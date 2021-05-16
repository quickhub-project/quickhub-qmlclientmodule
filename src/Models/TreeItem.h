/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */

#include <QVariant>
#include <QMap>

class TreeItem
{

public:
    explicit TreeItem(QString path, TreeItem *parentItem = 0);
    ~TreeItem();
    void appendChild(TreeItem *child);
    TreeItem *child(int row);
    TreeItem* hasChild(QString path);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void setData(QVariantList data);
    int row() const;
    TreeItem *parentItem();
    QString path();
    bool isDevice() const;
    void setIsDevice(bool isDevice);
    QString deviceMapping() const;
    void setDeviceMapping(const QString &deviceMapping);

private:
    QString m_path;
    QString m_deviceMapping;
    bool m_isDevice = false;
    QList<TreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem = 0;
    QMap<QString, TreeItem*> _childrenMap;
};
