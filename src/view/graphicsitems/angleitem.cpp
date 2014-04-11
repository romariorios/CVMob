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

#include "angleitem.hpp"

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QPen>

AngleItem::AngleItem(QGraphicsItem* parent) :
    QGraphicsItemGroup(parent),
    _center(new QGraphicsRectItem(this)),
    _edge1(new QGraphicsRectItem(this)),
    _edge2(new QGraphicsRectItem(this)),
    _line1(new QGraphicsLineItem(this)),
    _line2(new QGraphicsLineItem(this))
{
    addToGroup(_line1);
    addToGroup(_line2);
    addToGroup(_center);
    addToGroup(_edge1);
    addToGroup(_edge2);

    _edge1->setPen(Qt::NoPen);
    _edge1->setBrush(Qt::red);

    _edge2->setPen(Qt::NoPen);
    _edge2->setBrush(Qt::red);

    _center->setPen(Qt::NoPen);
    _center->setBrush(Qt::red);
}

AngleItem::AngleItem(const QPointF& center, const QPointF& edge1, const QPointF& edge2, QGraphicsItem* parent) :
    AngleItem(parent)
{
    setPoints(center, edge1, edge2);
}

QRectF rectFromPoint(const QPointF &point)
{
    return QRectF(point - QPointF(2, 2), QSizeF(4, 4));
}

void AngleItem::setPoints(const QPointF& center, const QPointF& edge1, const QPointF& edge2)
{
    _centerPoint = center;
    _edge1Point = edge1;
    _edge2Point = edge2;

    _center->setRect(rectFromPoint(center));
    _edge1->setRect(rectFromPoint(edge1));
    _edge2->setRect(rectFromPoint(edge2));

    _line1->setLine(QLineF(center, edge1));
    _line2->setLine(QLineF(center, edge2));
}

void AngleItem::setCenter(const QPointF& center)
{
    setPoints(center, _edge1Point, _edge2Point);
}

void AngleItem::setEdge1(const QPointF& edge1)
{
    setPoints(_centerPoint, edge1, _edge2Point);
}

void AngleItem::setEdge2(const QPointF& edge2)
{
    setPoints(_centerPoint, _edge1Point, edge2);
}
