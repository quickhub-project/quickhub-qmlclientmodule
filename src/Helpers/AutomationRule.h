/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#ifndef AUTOMATIONRULE_H
#define AUTOMATIONRULE_H

#include <QObject>
#include <QQmlListProperty>

class AutomationRule : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString ruleName READ ruleName WRITE setRuleName NOTIFY ruleNameChanged)
    Q_PROPERTY(QQmlListProperty<QObject> children READ children)
    Q_CLASSINFO("DefaultProperty", "children")

public:
    explicit AutomationRule(QObject *parent = 0);

    QString ruleName() const;
    void setRuleName(const QString &ruleName);

    QQmlListProperty<QObject> children();

signals:
    void ruleNameChanged();

public slots:

private:
    QString _ruleName;
    QList<QObject*> _children;
};

#endif // AUTOMATIONRULE_H
