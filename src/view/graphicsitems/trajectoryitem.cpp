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

#include "trajectoryitem.hpp"

#include "trajectoryinstantitem.hpp"

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>

TrajectoryItem::TrajectoryItem(QGraphicsItem *parent) :
    QGraphicsItem(parent),
    _drawTrajectory(DrawBefore),
    _showSpeed(ShowInCurrentInstant),
    _showAccel(ShowInCurrentInstant),
    _startingFrame(0),
    _currentFrame(0),
    _groupedLines(new QGraphicsItemGroup(this)),
    _linesBefore(new QGraphicsItemGroup(_groupedLines)),
    _linesAfter(new QGraphicsItemGroup(_groupedLines)),
    _currentInstant(0),
    _otherInstants(new QGraphicsItemGroup(this))
{
    _groupedLines->addToGroup(_linesBefore);
    _groupedLines->addToGroup(_linesAfter);

    followDrawPolicy();
    followShowSpeedPolicy();
    followShowAccelPolicy();
}

TrajectoryItem::~TrajectoryItem()
{
    delete _groupedLines;
    delete _otherInstants;
}

QRectF TrajectoryItem::boundingRect() const
{
    return _groupedLines->boundingRect();
}

void TrajectoryItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

TrajectoryItem::DrawingPolicy TrajectoryItem::drawTrajectory() const
{
    return _drawTrajectory;
}

void TrajectoryItem::setDrawTrajectory(TrajectoryItem::DrawingPolicy policy)
{
    if (_drawTrajectory == policy) {
        return;
    }

    _drawTrajectory = policy;

    followDrawPolicy();
}

TrajectoryItem::ShowPolicy TrajectoryItem::showSpeed() const
{
    return _showSpeed;
}

void TrajectoryItem::setShowSpeed(TrajectoryItem::ShowPolicy policy)
{
    _showSpeed = policy;

    followShowSpeedPolicy();
}

TrajectoryItem::ShowPolicy TrajectoryItem::showAccel() const
{
    return _showAccel;
}

void TrajectoryItem::setShowAccel(TrajectoryItem::ShowPolicy policy)
{
    _showAccel = policy;

    followShowAccelPolicy();
}

int TrajectoryItem::startingFrame() const
{
    return _startingFrame;
}

void TrajectoryItem::setStartingFrame(int frame)
{
    // TODO

    _startingFrame = frame;
}

void TrajectoryItem::setCurrentFrame(int frame)
{
    if (frame == _currentFrame ||
            frame < _startingFrame ||
            frame - _startingFrame >= _instants.size()) {
        return;
    }

    int curI = frame - _startingFrame;

    QGraphicsItem *prevInstant = _currentInstant;
    _currentInstant = _instants.at(curI);
    _otherInstants->removeFromGroup(_currentInstant);
    _otherInstants->addToGroup(prevInstant);

    _currentInstant->show();
    _otherInstants->hide();

    // Fast-forward
    for (; _currentFrame < frame; ++_currentFrame) {
        int li = frame - _startingFrame;

        _linesAfter->removeFromGroup(_lines.at(li));
        _linesBefore->addToGroup(_lines.at(li));
    }

    // Backwards
    for (; _currentFrame > frame; --_currentFrame) {
        int li = _currentFrame - _startingFrame;

        _linesBefore->removeFromGroup(_lines.at(li));
        _linesAfter->addToGroup(_lines.at(li));
    }
}

void TrajectoryItem::appendInstant(QPointF pos, QPointF speed, QPointF accel)
{
    TrajectoryInstantItem *instant = new TrajectoryInstantItem(pos, speed, accel, this);

    QGraphicsLineItem *lineBefore = 0;
    if (_lines.size() != 0) {
        lineBefore = new QGraphicsLineItem(instant);
    }
    _lines << lineBefore;
    _linesAfter->addToGroup(lineBefore);

    instant->setLineBefore(lineBefore);
    if (_instants.size() != 0) {
        _instants.last()->setInstantAfter(instant);
    }

    if (!_currentInstant) {
        _currentInstant = instant;
    } else {
        _otherInstants->addToGroup(instant);
    }

    _instants << instant;
}

void TrajectoryItem::followDrawPolicy()
{
    switch (_drawTrajectory) {
    case NoDraw:
        _groupedLines->hide();
        break;
    case DrawBefore:
        _groupedLines->show();
        _linesAfter->hide();
        break;
    case DrawBeforeAndAfter:
        _groupedLines->show();
        _linesAfter->show();
        break;
    }
}

void TrajectoryItem::followShowAccelPolicy()
{
    // TODO
}

void TrajectoryItem::followShowSpeedPolicy()
{
    // TODO
}
