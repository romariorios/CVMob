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

#include "distancesproxymodel.hpp"

#include <model/cvmobvideomodel.hpp>
#include <QItemSelectionModel>

#include <QDebug>

DistancesProxyModel::DistancesProxyModel(QObject *parent) :
    QIdentityProxyModel(parent)
{
}

QVariant DistancesProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!_parentIndex.isValid() ||
        role != Qt::DisplayRole ||
        proxyIndex.row() >= rowCount() ||
        proxyIndex.column() >= columnCount()) {
        return QVariant();
    }

    const QLineF line = sourceModel()
            ->data(sourceModel()->index(proxyIndex.row(), 0, _parentIndex), CvmobVideoModel::VideoSceneRole)
            .toLineF();

    switch (proxyIndex.column()) {
    case 0:
        return line.x1();
    case 1:
        return line.y1();
    case 2:
        return line.x2();
    case 3:
        return line.y2();
    default:
        break;
    }

    return QVariant();
}

QVariant DistancesProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal ||
        role != Qt::DisplayRole) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return tr("x1");
    case 1:
        return tr("x2");
    case 2:
        return tr("y1");
    case 3:
        return tr("y2");
    default:
        return QVariant();
    }
}

int DistancesProxyModel::rowCount(const QModelIndex &) const
{
    if (!_parentIndex.isValid()) {
        return 0;
    }

    return sourceModel()->rowCount(_parentIndex);
}

int DistancesProxyModel::columnCount(const QModelIndex &) const
{
    if (!_parentIndex.isValid()) {
        return 0;
    }

    return 4;
}

void DistancesProxyModel::setSelectionModel(QItemSelectionModel *selectionModel)
{
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

void DistancesProxyModel::selectionChanged(const QItemSelection &selected,
                                           const QItemSelection &deselected)
{
    Q_UNUSED(deselected)

    beginResetModel();

    if (selected.size() != 1 ||
        selected.at(0).indexes().size() != 1) {
        _parentIndex = QModelIndex();
    } else {
        _parentIndex = selected.at(0).indexes().at(0);
    }

    endResetModel();
}