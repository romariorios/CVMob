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

#include "baseproxymodel.hpp"

#include <QItemSelectionModel>
#include <QItemSelection>

BaseProxyModel::BaseProxyModel(QObject *parent) :
    QAbstractProxyModel(parent),
    _column(VideoModel::InvalidCol)
{
}

QModelIndex BaseProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex BaseProxyModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

void BaseProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractItemModel::dataChanged,
            [=](const QModelIndex &topLeft, const QModelIndex &bottomRight)
    {
        QModelIndex proxyTopLeft = mapFromSource(topLeft);
        QModelIndex proxyBottomRight = topLeft == bottomRight? proxyTopLeft :
                                                               mapFromSource(bottomRight);
        emit dataChanged(proxyTopLeft, proxyBottomRight);
    });

    connect(sourceModel, &QAbstractItemModel::rowsInserted, [=](const QModelIndex &parent,
                                                                int first, int last)
    {
        beginInsertRows(mapFromSource(parent), first, last);
        endInsertRows();
    });

    connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, [=](const QModelIndex &parent,
                                                                        int first, int last)
    {
        beginRemoveRows(mapFromSource(parent), first, last);
        endRemoveRows();
    });
}

void BaseProxyModel::setSelectionModel(QItemSelectionModel *selectionModel)
{
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            [=](const QItemSelection &selected)
    {
        beginResetModel();

        if (selected.size() != 1 ||
            selected.at(0).indexes().size() != 1 ||
            _column == VideoModel::InvalidCol) {
            _parentIndex = QModelIndex();
        } else {
            _parentIndex = sourceModel()->index(selected.at(0).indexes().at(0).row(), _column);
        }

        endResetModel();
    });
}

bool BaseProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    sourceModel()->removeRows(row, count, mapToSource(parent));

    endRemoveRows();
}

void BaseProxyModel::setColumn(int column)
{
    _column = column;
}
