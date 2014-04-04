/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "trajectoriesproxymodel.hpp"

TrajectoriesProxyModel::TrajectoriesProxyModel(QObject *parent) :
    InstantsProxyModel(parent)
{
    setColumn(VideoModel::TrajectoriesColumn);
}

QVariant TrajectoriesProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (role == VideoModel::SourceDataRole) {
        return mapToSource(proxyIndex).data();
    }
    
    if (role != Qt::DisplayRole ||
        !proxyIndex.isValid() ||
        !_parentIndex.isValid()) {
        return QVariant();
    }
    
    if (!proxyIndex.parent().isValid()) {
        return proxyIndex.column() == 0? tr("Trajectory %1").arg(proxyIndex.row()) : QVariant();
    }

    QVariant data = mapToSource(proxyIndex).data();

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

int TrajectoriesProxyModel::columnCount(const QModelIndex &parent) const
{
    return VideoModel::TrajectoryInstantColumnCount;
}
