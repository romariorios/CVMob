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

#include "instantsproxymodel.hpp"

InstantsProxyModel::InstantsProxyModel(QObject* parent) :
    BaseProxyModel(parent)
{}

QModelIndex InstantsProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (sourceIndex.parent() == _parentIndex) {
        return index(sourceIndex.row(), 0);
    }

    if (sourceIndex.parent().parent() == _parentIndex) {
        return index(sourceIndex.row(), sourceIndex.column(), index(sourceIndex.parent().row(), 0));
    }

    return QModelIndex();
}

QModelIndex InstantsProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!_parentIndex.isValid()) {
        return QModelIndex();
    }
    
    if (!proxyIndex.isValid()) {
        return _parentIndex;
    }

    if (!proxyIndex.parent().isValid()) {
        return sourceModel()->index(proxyIndex.row(), 0, _parentIndex);
    }

    QModelIndex parent = sourceModel()->index(proxyIndex.parent().row(), 0, _parentIndex);
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column(), parent);
}

QModelIndex InstantsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return createIndex(row, column, parent.row());
    }

    return createIndex(row, column, -1);
}

QModelIndex InstantsProxyModel::parent(const QModelIndex &child) const
{
    if (child.internalId() == -1) {
        return QModelIndex();
    }

    return index(child.internalId(), 0);
}

int InstantsProxyModel::rowCount(const QModelIndex &parent) const
{
    if (!_parentIndex.isValid()) {
        return 0;
    }

    if (!parent.isValid()) {
        return sourceModel()->rowCount(_parentIndex);
    }

    return sourceModel()->rowCount(mapToSource(parent));
}
