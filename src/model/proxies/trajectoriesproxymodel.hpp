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

#ifndef TRAJECTORIESPROXYMODEL_HPP
#define TRAJECTORIESPROXYMODEL_HPP

#include <model/proxies/instantsproxymodel.hpp>

class TrajectoriesProxyModel : public InstantsProxyModel
{
    Q_OBJECT
public:
    explicit TrajectoriesProxyModel(QObject *parent = 0);

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int mapColumnToSource(int column, const QModelIndex &parent = QModelIndex {}) const;
};

#endif // TRAJECTORIESPROXYMODEL_HPP
