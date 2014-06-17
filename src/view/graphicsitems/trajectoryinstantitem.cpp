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

#include "trajectoryinstantitem.hpp"

#include <QGraphicsLineItem>
#include <QBrush>

const QPointF radius = QPointF(2, 2);

TrajectoryInstantItem::TrajectoryInstantItem(QPointF pos, QPointF speed, QPointF accel,
                                             QGraphicsItem *parent) :
    QGraphicsRectItem(pos.x() - radius.x(), pos.y() - radius.y(),
                      radius.x() * 2, radius.y() * 2, parent),
    _speed(speed),
    _accel(accel),
    _speedLine(new QGraphicsLineItem(QLineF(pos, pos + speed), this)),
    _accelLine(new QGraphicsLineItem(QLineF(pos, pos + accel), this))
{
    setBrush(Qt::black);

    hide();
}

TrajectoryInstantItem::TrajectoryInstantItem(QGraphicsItem *parent) :
    TrajectoryInstantItem(QPointF(0, 0), QPointF(0, 0), QPointF(0, 0), parent)
{}

TrajectoryInstantItem::~TrajectoryInstantItem()
{
    delete _speedLine;
    delete _accelLine;
}

bool TrajectoryInstantItem::isSpeedVisible() const
{
    return _speedLine->isVisible();
}

bool TrajectoryInstantItem::isAccelVisible() const
{
    return _accelLine->isVisible();
}

const QPointF TrajectoryInstantItem::pos() const
{
    return rect().topLeft() + radius;
}

void TrajectoryInstantItem::setSpeedVisible(bool visible)
{
    if (visible == isSpeedVisible()) {
        return;
    }

    _speedLine->setVisible(visible);
}

void TrajectoryInstantItem::setAccelVisible(bool visible)
{
    if (visible == isAccelVisible()) {
        return;
    }

    _accelLine->setVisible(visible);
}

void TrajectoryInstantItem::setPos(const QPointF &pos)
{
    QRectF newRect(pos - radius, pos + radius);

    if (newRect == rect()) {
        return;
    }

    setRect(newRect);
}

void TrajectoryInstantItem::setSpeed(const QPointF &speed)
{
    if (speed == _speed) {
        return;
    }

    _speedLine->setLine(QLineF(pos(), speed));
    _speed = speed;
}

void TrajectoryInstantItem::setAccel(const QPointF &accel)
{
    if (accel == _accel) {
        return;
    }

    _accelLine->setLine(QLineF(pos(), accel));
    _accel = accel;
}
