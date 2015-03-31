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

#include "plotproxymodel.hpp"

enum {
    __HACKCurrentFrameCol = 2
};

using namespace std;

PlotProxyModel::PlotProxyModel(
    int parentColumn,
    const QString &graphTitle,
    int yColumn,
    int xColumn,
    const QString &yTitle,
    const function<QVariant(const QModelIndex &)> &yData,
    const function<QVariant(const QModelIndex &)> &xData,
    const QString &xTitle,
    QObject *parent
) :
    InstantsProxyModel{parent},
    _graphTitle{graphTitle},
    _yColumn{yColumn},
    _xColumn{xColumn},
    _yTitle{yTitle},
    _xTitle{xTitle},
    _yData{yData},
    _xData{xData}
{
    setColumn(parentColumn);
}

QVariant PlotProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.parent().isValid() && (
            proxyIndex.column() == __HACKCurrentFrameCol ||
            proxyIndex.column() == VideoModel::TrajectoryColorCol)) {
        return mapToSource(proxyIndex).data();
    }

    switch (proxyIndex.column()) {
    case 0:
        return _xData(mapToSource(proxyIndex));
    case 1:
        return _yData(mapToSource(proxyIndex));
    default:
        return QVariant();
    }
}

QVariant PlotProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (section) {
    case 0:
        return _graphTitle;
    case 1:
        return _xTitle;
    case 2:
        return _yTitle;
    default:
        return QVariant();
    }
}

QModelIndex PlotProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (_parentIndex.isValid() && !sourceIndex.parent().isValid()) {
        auto path = VideoModel::indexPath(_parentIndex);

        if (path[0].row != sourceIndex.row() || sourceIndex.column() != VideoModel::CurrentFrameCol) {
            return QModelIndex {};
        }

        return index(0, __HACKCurrentFrameCol);
    }

    QModelIndex instantsSourceIndex = InstantsProxyModel::mapFromSource(sourceIndex);
    if (!instantsSourceIndex.parent().isValid()) {
        return instantsSourceIndex;
    }

    if (instantsSourceIndex.column() == _xColumn) {
        return index(instantsSourceIndex.row(), 0, instantsSourceIndex.parent());
    }

    if (instantsSourceIndex.column() == _yColumn) {
        return index(instantsSourceIndex.row(), 1, instantsSourceIndex.parent());
    }

    return QModelIndex();
}

QModelIndex PlotProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!_parentIndex.isValid()) {
        return QModelIndex {};
    }

    int column;

    if (!proxyIndex.parent().isValid()) {
        if (proxyIndex.column() == __HACKCurrentFrameCol) {
            auto path = VideoModel::indexPath(_parentIndex);

            return sourceModel()->index(path[0].row, VideoModel::CurrentFrameCol);
        } else {
            return InstantsProxyModel::mapToSource(proxyIndex);
        }
    }

    switch (proxyIndex.column()) {
    case 0:
        column = _xColumn;
        break;
    case 1:
        column = _yColumn;
        break;
    default:
        return QModelIndex();
    }

    return InstantsProxyModel::mapToSource(index(proxyIndex.row(), column, proxyIndex.parent()));
}

int PlotProxyModel::columnCount(const QModelIndex &p) const
{
    return p.isValid()? 2 : 1;
}
