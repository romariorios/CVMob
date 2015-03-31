/*
    CVMob - Motion capture program
    Copyright (C) 2013--2015  The CVMob contributors

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

#ifndef PLOTPROXIES_H
#define PLOTPROXIES_H

#include <model/proxies/instantsproxymodel.hpp>

#include <functional>

class PlotProxyModel : public InstantsProxyModel
{
    Q_OBJECT

public:
    explicit PlotProxyModel(
        int parentColumn,
        const QString &graphTitle,
        int yColumn,
        int xColumn,
        const QString &yTitle,
        const std::function<QVariant(const QModelIndex &)> &yData =
            [](const QModelIndex &index) { return index.data(); },
        const std::function<QVariant(const QModelIndex &)> &xData =
            [](const QModelIndex &index) { return index.data(); },
        const QString &xTitle = tr("Frame"),
        QObject* parent = 0);
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    int columnCount(const QModelIndex &p = QModelIndex()) const;

private:
    const QString _graphTitle;
    const int _yColumn;
    const int _xColumn;
    const QString _yTitle;
    const QString _xTitle;
    const std::function<QVariant(const QModelIndex &)> _yData;
    const std::function<QVariant(const QModelIndex &)> _xData;
};

#endif // PLOTPROXIES_H
