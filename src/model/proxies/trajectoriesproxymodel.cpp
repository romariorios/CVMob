/*
    CVMob - Motion capture program
    Copyright (C) 2013, 2014  The CVMob contributors

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

enum {
    ClipboardFrameColumn = 0,
    ClipboardPositionXColumn,
    ClipboardPositionYColumn,
    ClipboardSpeedAbsColumn,
    ClipboardSpeedXColumn,
    ClipboardSpeedYColumn,
    ClipboardAccelerationAbsColumn,
    ClipboardAccelerationXColumn,
    ClipboardAccelerationYColumn,
    ClipboardColumnCount
};

TrajectoriesProxyModel::TrajectoriesProxyModel(QObject *parent) :
    InstantsProxyModel(parent)
{
    setColumn(VideoModel::AllTrajectoriesCol);
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

    QVariant data = mapToSource(proxyIndex).data();

    switch (proxyIndex.column()) {
    case ClipboardFrameColumn:
        return data;
    case ClipboardPositionXColumn:
    case ClipboardSpeedXColumn:
    case ClipboardAccelerationXColumn:
        return data.toPointF().x();
    case ClipboardPositionYColumn:
    case ClipboardSpeedYColumn:
    case ClipboardAccelerationYColumn:
        return data.toPointF().y();
    case ClipboardSpeedAbsColumn:
    case ClipboardAccelerationAbsColumn:
        return QLineF { QPointF { 0, 0 }, data.toPointF() }.length();
    default:
        return QVariant();
    }
    
    return QVariant {};
}

QVariant TrajectoriesProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole ||
        orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch (section) {
    case ClipboardFrameColumn:
        return tr("Frame");
    case ClipboardPositionXColumn:
        return tr("Position (x)");
    case ClipboardPositionYColumn:
        return tr("Position (y)");
    case ClipboardSpeedAbsColumn:
        return tr("Speed (abs)");
    case ClipboardSpeedXColumn:
        return tr("Speed (x)");
    case ClipboardSpeedYColumn:
        return tr("Speed (y)");
    case ClipboardAccelerationAbsColumn:
        return tr("Acceleration (abs)");
    case ClipboardAccelerationXColumn:
        return tr("Acceleration (x)");
    case ClipboardAccelerationYColumn:
        return tr("Acceleration (y)");
    default:
        return QVariant();
    }
    
    return QVariant {};
}

int TrajectoriesProxyModel::columnCount(const QModelIndex &parent) const
{
    return ClipboardColumnCount;
}

int TrajectoriesProxyModel::mapColumnToSource(int column, const QModelIndex& parent) const
{
    switch (column) {
    case ClipboardFrameColumn:
        return VideoModel::LFrameCol;
    case ClipboardPositionXColumn:
    case ClipboardPositionYColumn:
        return VideoModel::PositionCol;
    case ClipboardSpeedAbsColumn:
    case ClipboardSpeedXColumn:
    case ClipboardSpeedYColumn:
        return VideoModel::LSpeedCol;
    case ClipboardAccelerationAbsColumn:
    case ClipboardAccelerationXColumn:
    case ClipboardAccelerationYColumn:
        return VideoModel::LAccelCol;
    default:
        return -1; // Invalid
    }
}
