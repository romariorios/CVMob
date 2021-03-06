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

#ifndef TRAJECTORYINSTANTITEM_HPP
#define TRAJECTORYINSTANTITEM_HPP

#include <QGraphicsRectItem>

class QGraphicsLineItem;

class TrajectoryInstantItem : public QGraphicsRectItem
{
public:
    TrajectoryInstantItem(const QPointF &pos, const QPointF &speed, const QPointF &accel,
                          double scaleFactor, QGraphicsItem* parent);
    TrajectoryInstantItem(QGraphicsItem *parent);
    ~TrajectoryInstantItem();

    bool isSpeedVisible() const;
    bool isAccelVisible() const;
    const QPointF pos() const;
    inline const QPointF speed() const { return _speed; }
    inline const QPointF accel() const { return _accel; }

    void setSpeedVisible(bool visible);
    void setAccelVisible(bool visible);
    void setPos(const QPointF &pos);
    void setSpeed(const QPointF &speed);
    void setAccel(const QPointF &accel);

private:
    QPointF _speed;
    QPointF _accel;
    QPointF _radius;
    QGraphicsLineItem *_speedLine;
    QGraphicsLineItem *_accelLine;
};

#endif // TRAJECTORYINSTANTITEM_HPP
