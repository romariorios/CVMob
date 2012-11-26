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

#include <model/videomodel.hpp>
#include <QItemSelectionModel>

#include <QDebug>

DistancesProxyModel::DistancesProxyModel(QObject *parent) :
    QAbstractProxyModel(parent)
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

    const QLineF line = mapToSource(proxyIndex).data(VideoModel::VideoSceneRole).toLineF();

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

QModelIndex DistancesProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!_parentIndex.isValid() ||
        !sourceIndex.parent().isValid() ||
        !sourceIndex.parent().column() != VideoModel::DistancesColumn ||
        !sourceIndex.parent().row() != _parentIndex.row()) {
        return QModelIndex();
    }

    return index(sourceIndex.row(), 0);
}

QModelIndex DistancesProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!_parentIndex.isValid()) {
        return QModelIndex();
    }

    return sourceModel()->index(proxyIndex.row(), 0, _parentIndex);
}

QModelIndex DistancesProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex DistancesProxyModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

int DistancesProxyModel::rowCount(const QModelIndex &) const
{
    return _parentIndex.isValid()? sourceModel()->rowCount(_parentIndex) : 0;
}

int DistancesProxyModel::columnCount(const QModelIndex &) const
{
    return 4;
}

void DistancesProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(forwardDataChange(QModelIndex,QModelIndex)));
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
        _parentIndex = sourceModel()->index(selected.at(0).indexes().at(0).row(),
                                            VideoModel::DistancesColumn);
    }

    endResetModel();
}

void DistancesProxyModel::forwardDataChange(const QModelIndex &topLeft,
                                            const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}
