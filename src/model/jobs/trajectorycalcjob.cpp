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

#include "trajectorycalcjob.hpp"

#include <model/videomodel.hpp>

#include <QSize>

#include <QDebug>

using namespace std;

TrajectoryCalcJob::TrajectoryCalcJob(const QPointF &startPoint,
                                                 int startFrame,
                                                 int endFrame,
                                                 int videoRow,
                                                 const QSize &windowSize,
                                                 QAbstractItemModel *parent) :
    BaseJob({ startPoint }, startFrame, endFrame, videoRow, windowSize, parent),
    _previousSpeed(0, 0)
{
    connect(this, &TrajectoryCalcJob::instantGenerated,
            &_target, &TargetTrajectory::storeInstant, Qt::QueuedConnection);
    _target.model = parent;
}

void TrajectoryCalcJob::setTarget(const QModelIndex &targetIndex)
{
    if (isRunning() || isFinished()) {
        return;
    }

    _target.parentIndex = targetIndex;
}

void TrajectoryCalcJob::emitNewPoints(int frame,
                                      const QVector< QPointF >& points)
{
    QPointF newPoint = points.at(0);
    QPointF newSpeed = newPoint - _previousPoint;
    QPointF newAccel = newSpeed - _previousSpeed;
    
    emit instantGenerated(frame, newPoint, newSpeed, newAccel);
    
    _previousPoint = newPoint;
    _previousSpeed = newSpeed;
}

TargetTrajectory::TargetTrajectory(QObject *parent) :
    QObject(parent)
{}

void TargetTrajectory::storeInstant(int frame, const QPointF &p, const QPointF &s, const QPointF &a)
{
    if (!parentIndex.isValid()) {
        return;
    }

    int currentRow = model->rowCount(parentIndex);

    model->insertRow(currentRow, parentIndex);

    if (currentRow == 0) {
        model->insertColumns(0, VideoModel::TrajectoryInstantColumnCount, parentIndex);
    }

    model->setData(model->index(currentRow, VideoModel::LFrameColumn, parentIndex), frame,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::PositionColumn, parentIndex), p,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::LSpeedColumn, parentIndex), s,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::LAccelerationColumn, parentIndex), a,
                   VideoModel::VideoSceneEditRole);
}
