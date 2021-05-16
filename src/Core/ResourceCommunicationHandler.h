/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#ifndef ISYNCHRONIZEDBASEMODEL_H
#define ISYNCHRONIZEDBASEMODEL_H

#include <QObject>
#include "BaseCommunicationHandler.h"

class ResourceCommunicationHandler : public BaseCommunicationHandler
{
    Q_OBJECT

public:
    explicit ResourceCommunicationHandler(QString resourceType, QObject *parent = nullptr);
    void setDescriptor(const QString &resourceName);
    QString getDescriptor() const;

signals:
    void descriptorChanged();

private:
    void                p_attachModel();
    bool                _doReconnect = false;
    QString             _descriptor;
    QString             _attachedDescriptor;
    QString             _resourcePath;
    QString             _resourceType;
    bool                _shared = false;
    bool                _attachWhenReady = false;

public slots:
    void                attachModel() override;
    void                detachModel() override;

private slots:
    void                stateChangedSlot();

protected slots:
    virtual void messageReceived(QVariant message) override;

};

#endif // ISYNCHRONIZEDBASEMODEL_H
