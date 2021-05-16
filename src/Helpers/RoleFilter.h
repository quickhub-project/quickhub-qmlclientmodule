/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * It is part of the QuickHub framework - www.quickhub.org
 * Copyright (C) 2021 by Friedemann Metzger - mail@friedemann-metzger.de */


#ifndef SHOPPINGLISTFILTER_H
#define SHOPPINGLISTFILTER_H

#include <QObject>
#include <QSortFilterProxyModel>

class RoleFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString booleanFilterRoleName READ booleanFilterRoleName WRITE setBooleanFilterRoleName NOTIFY booleanFilterRoleNameChanged)
    Q_PROPERTY(QString numericFilterRoleName READ getNumericFilterRoleName WRITE setNumericFilterRoleName NOTIFY numericFilterRoleNameChanged)
    Q_PROPERTY(QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged)
    Q_PROPERTY(QString stringFilterSearchRole READ stringFilterSearchRole WRITE setStringFilterSearchRole NOTIFY stringFilterSearchRoleChanged)
    Q_PROPERTY(QString booleanSortRoleName READ booleanSortRoleName WRITE setBooleanSortRoleName NOTIFY booleanSortRoleNameChanged)
    Q_PROPERTY(QString sortRoleString READ sortRoleString WRITE setSortRoleString NOTIFY sortRoleStringChanged)
    Q_PROPERTY(double numericFilterThreshold READ getNumericFilterThreshold WRITE setNumericFilterThreshold NOTIFY numericFilterThresholdChanged)
    Q_PROPERTY(bool inverse READ inverse WRITE setInverse NOTIFY inverseChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    explicit RoleFilter(QObject *parent = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

    QString booleanFilterRoleName() const;
    void setBooleanFilterRoleName(const QString &booleanFilterRoleName);

    QHash<int, QByteArray> roleNames() const;

    bool inverse() const;
    void setInverse(bool inverse);
    Q_INVOKABLE int getSourceIndex(int index);

    QString booleanSortRoleName() const;
    void setBooleanSortRoleName(const QString &booleanSortRoleName);

    QString searchString() const;
    void setSearchString(const QString &sarchString);

    QString sortRoleString();
    void setSortRoleString(QString sortRole);

    QString stringFilterSearchRole() const;
    void setStringFilterSearchRole(const QString &stringFilterSearchRole);

    QString getNumericFilterRoleName() const;
    void setNumericFilterRoleName(const QString &numericFilterRoleName);

    double getNumericFilterThreshold() const;
    void setNumericFilterThreshold(double numericFilterValue);

private:
    QString _sortRoleString;
    QString _searchString;
    QString _stringFilterSearchRole;
    QString _booleanFilterRoleName;
    QString _numericFilterRoleName;
    double  _numericFilterThreshold;
    QString _booleanSortRoleName;
    bool    _inverse;

private slots:


signals:
    void sortRoleStringChanged();
    void booleanFilterRoleNameChanged();
    void numericFilterRoleNameChanged();
    void booleanSortRoleNameChanged();
    void stringFilterSearchRoleChanged();
    void inverseChanged();
    void countChanged();
    void searchStringChanged();
    void numericFilterThresholdChanged();

public slots:
};

#endif // SHOPPINGLISTFILTER_H
