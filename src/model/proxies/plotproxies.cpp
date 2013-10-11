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

#include "plotproxies.hpp"

BasePlotProxyModel::BasePlotProxyModel(QObject* parent) :
    InstantsProxyModel(parent)
{}

QVariant BasePlotProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
    switch (proxyIndex.column()) {
    case 0:
        return xData(mapToSource(proxyIndex));
    case 1:
        return yData(mapToSource(proxyIndex));
    default:
        return QVariant();
    }
}

QVariant BasePlotProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (section) {
    case 0:
        return graphTitle();
    case 1:
        return xTitle();
    case 2:
        return yTitle();
    default:
        return QVariant();
    }
}

QModelIndex BasePlotProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    QModelIndex instantsSourceIndex = InstantsProxyModel::mapFromSource(sourceIndex);
    if (!instantsSourceIndex.parent().isValid()) {
        return instantsSourceIndex;
    }
    
    if (instantsSourceIndex.column() == xColumn()) {
        return index(instantsSourceIndex.row(), 0, instantsSourceIndex.parent());
    }
    
    if (instantsSourceIndex.column() == yColumn()) {
        return index(instantsSourceIndex.row(), 1, instantsSourceIndex.parent());
    }
    
    return QModelIndex();
}

QModelIndex BasePlotProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    int column;
    
    switch (proxyIndex.column()) {
    case 0:
        column = xColumn();
        break;
    case 1:
        column = yColumn();
        break;
    default:
        return QModelIndex();
    }
    
    return InstantsProxyModel::mapToSource(index(proxyIndex.row(), column, proxyIndex.parent()));
}

int BasePlotProxyModel::columnCount(const QModelIndex& p) const
{
    return p.isValid()? 2 : 1;
}
