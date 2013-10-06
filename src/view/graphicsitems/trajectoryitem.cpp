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

#include <QGraphicsLineItem>

TrajectoryItem::TrajectoryItem(QGraphicsItem *parent) :
    QGraphicsItemGroup(parent),
    _drawTrajectory(DrawBefore),
    _showSpeed(ShowInCurrentInstant),
    _showAccel(ShowInCurrentInstant),
    _startingFrame(0),
    _currentFrame(0),
    _linesBefore(new QGraphicsItemGroup(this)),
    _linesAfter(new QGraphicsItemGroup(this)),
    _currentInstant(0)
{
    addToGroup(_linesBefore);
    addToGroup(_linesAfter);

    followDrawPolicy();
    followShowSpeedPolicy();
    followShowAccelPolicy();
}

TrajectoryItem::~TrajectoryItem()
{}

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
    QGraphicsItem *prevInstant = _currentInstant;
    _currentInstant = instantAt(frame - _startingFrame);
    
    _currentInstant? _currentInstant->show() : void();
    prevInstant? prevInstant->hide() : void();
    
    // Fast-forward
    for (; _currentFrame < frame; ++_currentFrame) {
        int curL = _currentFrame - _startingFrame;
        if (curL < 0 || curL >= _lines.size()) {
            continue;
        }
        
        _linesAfter->removeFromGroup(_lines[curL]);
        _linesBefore->addToGroup(_lines[curL]);
    }
    
    // Backwards
    for (; _currentFrame > frame; --_currentFrame) {
        int curL = _currentFrame - _startingFrame;
        if (curL < 0 || curL >= _lines.size()) {
            continue;
        }
        
        _linesBefore->removeFromGroup(_lines[curL]);
        _linesAfter->addToGroup(_lines[curL]);
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
        _currentInstant->show();
    }

    _instants << instant;
}

TrajectoryInstantItem* TrajectoryItem::instantAt(int pos) const
{
    return pos < 0 || pos >= _instants.size()? nullptr : _instants.at(pos);
}

void TrajectoryItem::followDrawPolicy()
{
    switch (_drawTrajectory) {
    case NoDraw:
        break;
    case DrawBefore:
        _linesAfter->hide();
        break;
    case DrawBeforeAndAfter:
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
