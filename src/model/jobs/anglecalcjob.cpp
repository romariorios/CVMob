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

#include "anglecalcjob.hpp"

#include <model/videomodel.hpp>

using namespace std;

AngleCalcJob::AngleCalcJob(const QVector< QPointF >& startAngle,
                           int startFrame, int endFrame, int videoRow,
                           QAbstractItemModel* parent) :
    BaseJob(startAngle, startFrame, endFrame, videoRow, parent),
    _previousAngle(angleFromPoints(startAngle[0], startAngle[1], startAngle[2])),
    _previousASpeed(0)
{
    connect(this, &AngleCalcJob::instantGenerated,
            &_target, &TargetAngle::storeInstant, Qt::QueuedConnection);
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
    BaseTarget(parent)
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
        model->insertColumns(0, VideoModel::AngleInstantColCount, parentIndex);
    }

    model->setData(model->index(currentRow, VideoModel::AFrameCol, parentIndex), frame);
    model->setData(model->index(currentRow, VideoModel::ASpeedCol, parentIndex), aSpeed);
    model->setData(model->index(currentRow, VideoModel::AAccelCol, parentIndex), aAccel);
    model->setData(model->index(currentRow, VideoModel::ACenterCol, parentIndex),
                   centralEdge);
    model->setData(model->index(currentRow, VideoModel::AEdge1Col, parentIndex),
                   pEdge1);
    model->setData(model->index(currentRow, VideoModel::AEdge2Col, parentIndex),
                   pEdge2);
}
