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

#include "basejob.hpp"

BaseTarget::BaseTarget(QObject* parent) :
    QObject(parent),
    model(0)
{}

BaseJob::BaseJob(const QVector< QPointF >& startPoints, int startFrame, int endFrame, int videoRow, QAbstractItemModel* parent) :
    QObject(parent),
    _currentPoints(startPoints),
    _startFrame(startFrame),
    _endFrame(endFrame),
    _videoRow(videoRow),
    _currentFrame(startFrame),
    _model(parent)
{}

void BaseJob::setTarget(const QModelIndex& index)
{
    target().model = _model;
    target().parentIndex = index;
}
