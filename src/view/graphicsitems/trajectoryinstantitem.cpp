#include "trajectoryinstantitem.hpp"

#include <QGraphicsLineItem>

const QPointF radius = QPointF(1, 1);

TrajectoryInstantItem::TrajectoryInstantItem(QPointF pos, QPointF speed, QPointF accel,
                                             QGraphicsItem *parent,
                                             QGraphicsLineItem *lineBefore,
                                             TrajectoryInstantItem *instantAfter) :
    QGraphicsRectItem(pos.x() - radius.x(), pos.y() - radius.y(),
                      radius.x() * 2, radius.y() * 2, parent),
    _speed(speed),
    _accel(accel),
    _speedLine(new QGraphicsLineItem(QLineF(pos, pos + speed), this)),
    _accelLine(new QGraphicsLineItem(QLineF(pos, pos + accel), this)),
    _lineBefore(lineBefore),
    _instantAfter(instantAfter)
{}

TrajectoryInstantItem::TrajectoryInstantItem(QGraphicsItem *parent) :
    TrajectoryInstantItem(QPointF(0, 0), QPointF(0, 0), QPointF(0, 0), parent)
{}

TrajectoryInstantItem::~TrajectoryInstantItem()
{
    delete _instantAfter->_lineBefore;
    _instantAfter->_lineBefore = _lineBefore;
    _lineBefore->setLine(QLineF(_lineBefore->line().p1(), _instantAfter->pos()));
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

    QLineF l;
    if (_lineBefore) {
        l = _lineBefore->line();
        _lineBefore->setLine(QLineF(l.p1(), pos));
    }

    if (_instantAfter) {
        l = _instantAfter->_lineBefore->line();
        _instantAfter->_lineBefore->setLine(QLineF(pos, l.p2()));
    }
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

void TrajectoryInstantItem::setLineBefore(QGraphicsLineItem *lineBefore)
{
    if (!_lineBefore) {
        _lineBefore = lineBefore;
    }
}

void TrajectoryInstantItem::setInstantAfter(TrajectoryInstantItem *instantAfter)
{
    if (!_instantAfter) {
        _instantAfter = instantAfter;
    }
}
