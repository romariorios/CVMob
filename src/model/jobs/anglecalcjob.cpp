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

#include "anglecalcjob.hpp"

#include <model/videomodel.hpp>

using namespace std;

AngleCalcJob::AngleCalcJob(const QVector< QPointF >& startAngle,
                           int startFrame, int endFrame, int videoRow,
                           const QSize& windowSize, QAbstractItemModel* parent) :
    BaseJob(startAngle, startFrame, endFrame, videoRow, windowSize, parent),
    _previousAngle(angleFromPoints(startAngle[0], startAngle[1], startAngle[2])),
    _previousASpeed(0)
{
    connect(this, &AngleCalcJob::instantGenerated,
            &_target, &TargetAngle::storeInstant);
    _target.model = parent;
}

void AngleCalcJob::setTarget(const QModelIndex &targetIndex)
{
    if (isRunning() || isFinished()) {
        return;
    }

    _target.parentIndex = targetIndex;
}

void AngleCalcJob::emitNewPoints(int frame, const QVector< QPointF >& points)
{
    float newAngle = angleFromPoints(points[0], points[1], points[2]);
    float newASpeed = newAngle - _previousAngle;
    float newAAccel = newASpeed - _previousASpeed;
    
    emit instantGenerated(frame, newASpeed, newAAccel, points[0], points[1], points[2]);
    
    _previousAngle = newAngle;
    _previousASpeed = newASpeed;
}

TargetAngle::TargetAngle(QObject* parent) :
    QObject(parent)
{}

void TargetAngle::storeInstant(int frame, float aSpeed, float aAccel, const QPointF& centralEdge,
                               const QPointF& pEdge1, const QPointF& pEdge2)
{
    if (!parentIndex.isValid()) {
        return;
    }

    int currentRow = model->rowCount(parentIndex);

    model->insertRow(currentRow, parentIndex);

    if (currentRow == 0) {
        model->insertColumns(0, VideoModel::AngleInstantColumnCount, parentIndex);
    }

    model->setData(model->index(currentRow, VideoModel::AFrameColumn, parentIndex), frame,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::ASpeedColumn, parentIndex), aSpeed,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::AAccelerationColumn, parentIndex), aAccel,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::CentralEdgeColumn, parentIndex),
                   centralEdge, VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::PeripheralEdge1Column, parentIndex),
                   pEdge1, VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::PeripheralEdge2Column, parentIndex),
                   pEdge2, VideoModel::VideoSceneEditRole);
}
