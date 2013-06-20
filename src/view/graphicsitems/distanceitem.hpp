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

#ifndef DISTANCEITEM_HPP
#define DISTANCEITEM_HPP

#include <QGraphicsLineItem>

class DistanceItem : public QGraphicsLineItem
{
public:
    DistanceItem(QLineF line, QGraphicsItem *parent = nullptr);
    DistanceItem(QGraphicsItem *parent = nullptr);
    ~DistanceItem();

    void setLine(const QLineF &l);
    inline void setP1(const QPointF &p1) { setLine(QLineF(p1, line().p2())); }
    inline void setP2(const QPointF &p2) { setLine(QLineF(line().p1(), p2)); }

private:
    QGraphicsRectItem *_p1;
    QGraphicsRectItem *_p2;
};

#endif // DISTANCEITEM_HPP
