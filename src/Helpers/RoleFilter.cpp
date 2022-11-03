/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#include "RoleFilter.h"
#include <QDebug>
RoleFilter::RoleFilter(QObject *parent) : QSortFilterProxyModel(parent)
{
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()), this, SIGNAL(countChanged()));
}

bool RoleFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    QModelIndex index = sourceModel()->index(source_row, 0);

    if(!_booleanFilterRoleName.isEmpty())
    {

        int role = sourceModel()->roleNames().key(_booleanFilterRoleName.toLatin1());

        if(_inverse)
            return !index.data(role).toBool();
        else
            return index.data(role).toBool();
    }

    if(!_numericFilterRoleName.isEmpty())
    {
        int role = sourceModel()->roleNames().key(_numericFilterRoleName.toLatin1());
        bool ok;
        double val = index.data(role).toDouble(&ok);
        if(ok && val >= _numericFilterThreshold)
        {
            return false;
        }

    }


    if( !_searchString.isEmpty())
    {
        QStringList roles = _stringFilterSearchRole.split(",");
        QListIterator<QString> it(roles);
        while(it.hasNext())
        {
            QString role = it.next().trimmed();
            int roleIndex = sourceModel()->roleNames().key(role.toLatin1());

            QVariant data = index.data(roleIndex);
            if(data.canConvert(QMetaType::QStringList))
            {
               QStringList items = data.toStringList();
               QListIterator<QString> itemIt(items);
               while(itemIt.hasNext())
               {
                   QString item = itemIt.next();
                   if(_findOnlyDirectMatches)
                   {
                       if(item == _searchString)
                           return true;
                   }
                   else
                   {
                       if(item.contains(_searchString, Qt::CaseInsensitive))
                           return true;
                   }
               }
            }
            else if(data.canConvert(QMetaType::QString))
            {
                QString item = data.toString();
                if(_findOnlyDirectMatches)
                {
                    if(item == _searchString)
                        return true;
                }
                else
                {
                    if(item.contains(_searchString, Qt::CaseInsensitive))
                        return true;
                }
            }
        }
        return false;
    }



    return true;
}

bool RoleFilter::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if(!_sortRoleString.isEmpty())
    {
        int role = sourceModel()->roleNames().key(_sortRoleString.toLatin1());
        QVariant l = source_left.data(role);
        QVariant r = source_right.data(role);

        if(l.canConvert<int>()){
            if(_inverse)
                return l.toInt() < r.toInt();
            else
                return r.toInt() < l.toInt();
        }
        else if(l.canConvert<float>()){
            if(_inverse)
                return l.toFloat() < r.toFloat();
            else
                return r.toFloat() < l.toFloat();
        }
        else if(l.canConvert<QString>()){
            if(_inverse)
                return l.toString() < r.toString();
            else
                return r.toString() < l.toString();
        }

    }

    if(!_booleanSortRoleName.isEmpty())
    {
        int role = sourceModel()->roleNames().key(_booleanSortRoleName.toLatin1());

        bool l = source_left.data(role).toBool();
        bool r = source_right.data(role).toBool();

        if(l && !r)
            return true;
        return false;
    }
    return false;
}

QString RoleFilter::booleanFilterRoleName() const
{
    return _booleanFilterRoleName;
}

void RoleFilter::setBooleanFilterRoleName(const QString &booleanFilterRoleName)
{
    if(_booleanFilterRoleName == booleanFilterRoleName)
        return;

    _booleanFilterRoleName = booleanFilterRoleName;
    Q_EMIT booleanFilterRoleNameChanged();
}

QHash<int, QByteArray> RoleFilter::roleNames() const
{
    QHash<int, QByteArray>  roles;
    if(sourceModel())
    {
        roles = sourceModel()->roleNames();
    }
    return roles;
}

bool RoleFilter::inverse() const
{
    return _inverse;
}

void RoleFilter::setInverse(bool inverse)
{
    if( _inverse == inverse)
        return;

    _inverse = inverse;
    Q_EMIT inverseChanged();
}

int RoleFilter::getSourceIndex(int index)
{

    QModelIndex proxyIndex = this->index(index, 0);
    if(proxyIndex.isValid())
    {
        return mapToSource(proxyIndex).row();
    }

    return -1;
}

QString RoleFilter::booleanSortRoleName() const
{
    return _booleanSortRoleName;
}

void RoleFilter::setBooleanSortRoleName(const QString &booleanSortRoleName)
{
    if(booleanSortRoleName.isEmpty())
        return;

    this->setDynamicSortFilter(true);
    this->sort(0);
     _booleanSortRoleName = booleanSortRoleName;
    Q_EMIT booleanSortRoleNameChanged();
}

QString RoleFilter::searchString() const
{
    return _searchString;
}

void RoleFilter::setSearchString(const QString &sarchString)
{
    _searchString = sarchString;
    Q_EMIT searchStringChanged();
    invalidateFilter();
}

QString RoleFilter::sortRoleString()
{
    return _sortRoleString;
}

void RoleFilter::setSortRoleString(QString sortRole)
{
    this->setDynamicSortFilter(true);
    this->sort(0);
    _sortRoleString = sortRole;
    Q_EMIT sortRoleStringChanged();
}

QString RoleFilter::stringFilterSearchRole() const
{
    return _stringFilterSearchRole;
}

void RoleFilter::setStringFilterSearchRole(const QString &stringFilterSearchRole)
{
    _stringFilterSearchRole = stringFilterSearchRole;
    Q_EMIT stringFilterSearchRoleChanged();
}

QString RoleFilter::getNumericFilterRoleName() const
{
    return _numericFilterRoleName;
}

void RoleFilter::setNumericFilterRoleName(const QString &numericFilterRoleName)
{
    _numericFilterRoleName = numericFilterRoleName;
    invalidateFilter();
    Q_EMIT numericFilterRoleNameChanged();
}

double RoleFilter::getNumericFilterThreshold() const
{
    return _numericFilterThreshold;
}

void RoleFilter::setNumericFilterThreshold(double numericFilterValue)
{
    invalidateFilter();
    _numericFilterThreshold = numericFilterValue;
    Q_EMIT numericFilterThresholdChanged();
}

bool RoleFilter::findOnlyDirectMatches() const
{
    return _findOnlyDirectMatches;
}

void RoleFilter::setFindOnlyDirectMatches(bool newFindOnlyDirectMatches)
{
    if (_findOnlyDirectMatches == newFindOnlyDirectMatches)
        return;
    _findOnlyDirectMatches = newFindOnlyDirectMatches;
    emit findOnlyDirectMatchesChanged();
}

