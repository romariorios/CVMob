/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

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

#include "trajectoriesproxymodel.hpp"

TrajectoriesProxyModel::TrajectoriesProxyModel(QObject *parent) :
    BaseProxyModel(parent)
{
    setColumn(VideoModel::TrajectoriesColumn);
}

QVariant TrajectoriesProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (role != Qt::DisplayRole ||
        !proxyIndex.isValid() ||
        !_parentIndex.isValid()) {
        return QVariant();
    }

    if (!proxyIndex.parent().isValid()) {
        return proxyIndex.column() == 0? tr("Trajectory %1").arg(proxyIndex.row()) : QVariant();
    }

    QVariant data = mapToSource(proxyIndex).data(VideoModel::VideoSceneEditRole);

    switch (proxyIndex.column()) {
    case VideoModel::LFrameColumn:
        return data;
    case VideoModel::PositionColumn:
        return tr("(%1, %2)").arg(data.toPointF().x()).arg(data.toPointF().y());
    case VideoModel::LSpeedColumn:
    case VideoModel::LAccelerationColumn:
        return QLineF(QPointF(0, 0), data.toPointF()).length();
    default:
        return QVariant();
    }
}

QVariant TrajectoriesProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole ||
        orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
    case VideoModel::LFrameColumn:
        return tr("Frame");
    case VideoModel::PositionColumn:
        return tr("Position");
    case VideoModel::LSpeedColumn:
        return tr("Speed");
    case VideoModel::LAccelerationColumn:
        return tr("Acceleration");
    default:
        return QVariant();
    }
}

QModelIndex TrajectoriesProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (sourceIndex.parent() == _parentIndex) {
        return index(sourceIndex.row(), 0);
    }

    if (sourceIndex.parent().parent() == _parentIndex) {
        return index(sourceIndex.row(), sourceIndex.column(), index(sourceIndex.parent().row(), 0));
    }

    return QModelIndex();
}

QModelIndex TrajectoriesProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!_parentIndex.isValid() ||
        !proxyIndex.isValid()) {
        return QModelIndex();
    }

    if (!proxyIndex.parent().isValid()) {
        return sourceModel()->index(proxyIndex.row(), 0, _parentIndex);
    }

    QModelIndex parent = sourceModel()->index(proxyIndex.parent().row(), 0, _parentIndex);
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column(), parent);
}

QModelIndex TrajectoriesProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return createIndex(row, column, parent.row());
    }

    return createIndex(row, column, -1);
}

QModelIndex TrajectoriesProxyModel::parent(const QModelIndex &child) const
{
    if (child.internalId() == -1) {
        return QModelIndex();
    }

    return index(child.internalId(), 0);
}

int TrajectoriesProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!_parentIndex.isValid()) {
        return 0;
    }

    if (!parent.isValid()) {
        return sourceModel()->rowCount(_parentIndex);
    }

    return sourceModel()->rowCount(mapToSource(parent));
}

int TrajectoriesProxyModel::columnCount(const QModelIndex &parent) const
{
    return VideoModel::TrajectoryInstantColumnCount;
}
