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

#ifndef VIDEOLISTPROXYMODEL_HPP
#define VIDEOLISTPROXYMODEL_HPP

#include <QIdentityProxyModel>

class VideoListProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit VideoListProxyModel(QObject* parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    inline int columnCount(const QModelIndex & = QModelIndex()) const { return 1; }
};

#endif
