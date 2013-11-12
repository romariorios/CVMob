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

#include "trajectorycalcjob.hpp"

#include <model/videomodel.hpp>

using namespace std;

TrajectoryCalcJob::TrajectoryCalcJob(const QPointF &startPoint,
                                                 int startFrame,
                                                 int endFrame,
                                                 int videoRow,
                                                 const QSize &windowSize,
                                                 QAbstractItemModel *parent) :
    BaseJob({ startPoint }, startFrame, endFrame, videoRow, windowSize, parent),
    _previousPoint(startPoint),
    _previousSpeed(0, 0)
{
    connect(this, &TrajectoryCalcJob::instantGenerated,
            &_target, &TargetTrajectory::storeInstant, Qt::QueuedConnection);
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
    BaseTarget(parent)
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

    model->setData(model->index(currentRow, VideoModel::LFrameColumn, parentIndex), frame);
    model->setData(model->index(currentRow, VideoModel::PositionColumn, parentIndex), p);
    model->setData(model->index(currentRow, VideoModel::LSpeedColumn, parentIndex), s);
    model->setData(model->index(currentRow, VideoModel::LAccelerationColumn, parentIndex), a);
}
