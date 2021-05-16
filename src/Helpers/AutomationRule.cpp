/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "AutomationRule.h"

AutomationRule::AutomationRule(QObject *parent) : QObject(parent)
{

}

QString AutomationRule::ruleName() const
{
    return _ruleName;
}

void AutomationRule::setRuleName(const QString &ruleName)
{
    _ruleName = ruleName;
    Q_EMIT ruleNameChanged();
}

QQmlListProperty<QObject> AutomationRule::children()
{
    return QQmlListProperty<QObject>(this, _children);
}
