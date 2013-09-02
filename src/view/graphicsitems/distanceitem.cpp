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

#include "distanceitem.hpp"

#include <QBrush>
#include <QPen>

#include <QDebug>

static const QPointF radius(2, 2);

DistanceItem::DistanceItem(QLineF line, QGraphicsItem *parent) :
    QGraphicsLineItem(parent),
    _p1(new QGraphicsRectItem(this)),
    _p2(new QGraphicsRectItem(this))
{
    setLine(line);
    _p1->show();
    _p1->setBrush(Qt::red);
    _p1->setPen(Qt::NoPen);
    _p2->show();
    _p2->setBrush(Qt::red);
    _p2->setPen(Qt::NoPen);
}

DistanceItem::DistanceItem(QGraphicsItem *parent) :
    DistanceItem(QLineF(0, 0, 0, 0), parent)
{}

DistanceItem::~DistanceItem()
{
    delete _p1;
    delete _p2;
}

void DistanceItem::setLine(const QLineF &l)
{
    QGraphicsLineItem::setLine(l);

    _p1->setRect(QRectF(l.p1() - radius, l.p1() + radius));
    _p2->setRect(QRectF(l.p2() - radius, l.p2() + radius));
}
