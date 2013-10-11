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

#ifndef DISTANCESPROXYMODEL_HPP
#define DISTANCESPROXYMODEL_HPP

#include <model/proxies/baseproxymodel.hpp>

class QItemSelectionModel;

class DistancesProxyModel : public BaseProxyModel
{
    Q_OBJECT
public:
    explicit DistancesProxyModel(QObject *parent = 0);

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    int rowCount(const QModelIndex & = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const;
};

#endif // DISTANCESPROXYMODEL_HPP
