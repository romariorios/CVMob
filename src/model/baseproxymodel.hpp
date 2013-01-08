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

#ifndef BASEPROXYMODEL_HPP
#define BASEPROXYMODEL_HPP

#include <QAbstractProxyModel>

#include <model/videomodel.hpp>

class QItemSelectionModel;

class BaseProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit BaseProxyModel(QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    void setSourceModel(QAbstractItemModel *sourceModel);
    void setSelectionModel(QItemSelectionModel *selectionModel);

protected:
    void setColumn(int column);

    QModelIndex _parentIndex;

private:
    int _column;

private slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void forwardDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void forwardRowInsertion(const QModelIndex &parent, int start, int end);
};

#endif // BASEPROXYMODEL_HPP
