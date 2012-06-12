/*
    CVMob - Motion capture program
    Copyright (C) 2012  The CVMob contributors

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef DISTANCESPROXYMODEL_HPP
#define DISTANCESPROXYMODEL_HPP

#include <QIdentityProxyModel>

class QItemSelectionModel;

class DistancesProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit DistancesProxyModel(QObject *parent = 0);

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const;
    void setSelectionModel(QItemSelectionModel *selectionModel);

private:
    QModelIndex _parentIndex;

private slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif // DISTANCESPROXYMODEL_HPP