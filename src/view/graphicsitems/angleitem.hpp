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

#ifndef ANGLEITEM_HPP
#define ANGLEITEM_HPP

#include <QGraphicsItemGroup>

class QGraphicsLineItem;
class QGraphicsRectItem;

class AngleItem : public QGraphicsItemGroup
{
public:
    explicit AngleItem(QGraphicsItem* parent = 0);
    AngleItem(const QPointF &center, const QPointF &edge1, const QPointF &edge2, QGraphicsItem* parent = 0);
    
    void setPoints(const QPointF &center, const QPointF &edge1, const QPointF &edge2);
    void setCenter(const QPointF &center);
    void setEdge1(const QPointF &edge1);
    void setEdge2(const QPointF &edge2);
    
    inline const QPointF center() { return _centerPoint; }
    inline const QPointF edge1() { return _edge1Point; }
    inline const QPointF edge2() { return _edge2Point; }
    
private:
    QGraphicsRectItem *_center;
    QGraphicsRectItem *_edge1;
    QGraphicsRectItem *_edge2;
    QGraphicsLineItem *_line1;
    QGraphicsLineItem *_line2;
    
    QPointF _centerPoint;
    QPointF _edge1Point;
    QPointF _edge2Point;
};

#endif
