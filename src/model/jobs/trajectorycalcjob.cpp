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

using namespace cv;
using namespace std;

TrajectoryCalcJob::TrajectoryCalcJob(const QPointF &startPoint,
                                                 int startFrame,
                                                 int endFrame,
                                                 int videoRow,
                                                 const QSize &windowSize,
                                                 QAbstractItemModel *parent) :
    BaseJob(parent),
    _startPoint(startPoint),
    _startFrame(startFrame),
    _endFrame(endFrame),
    _videoRow(videoRow),
    _windowSize(windowSize),
    _model(parent)
{
    connect(this, &TrajectoryCalcJob::instantGenerated,
            &_target, &Target::storeInstant, Qt::QueuedConnection);
    _target.model = _model;
    _framesParentIndex = _model->index(_videoRow, VideoModel::FramesColumn);
}

void TrajectoryCalcJob::setTarget(const QModelIndex &targetIndex)
{
    if (isRunning() || isFinished()) {
        return;
    }

    _target.parentIndex = targetIndex;
}

void TrajectoryCalcJob::run()
{
    QModelIndex framesParentIndex = _model->index(_videoRow, VideoModel::FramesColumn);

    emit instantGenerated(0, _startPoint, QPointF(0, 0), QPointF(0, 0));
    emit progressRangeChanged(0, _endFrame - _startFrame);
    emit progressChanged(0);

    QPointF previousPoint = _startPoint;
    QPointF previousSpeed(0, 0);

    for (int i = _startFrame + 1; i <= _endFrame; ++i) {
        QPointF newPoint = trackPoints(
            { previousPoint },
            _model->index(i - 1, 0, framesParentIndex).data(VideoModel::VideoSceneRole)
                          .value<QImage>(),
            _model->index(i, 0, framesParentIndex).data(VideoModel::VideoSceneRole)
                        .value<QImage>()
        ).at(0);
        QPointF newSpeed = newPoint - previousPoint;
        QPointF newAccel = newSpeed - previousSpeed;

        emit instantGenerated(i, newPoint, newSpeed, newAccel);
        emit progressChanged(i - _startFrame + 1);

        previousPoint = newPoint;
        previousSpeed = newSpeed;
    }
}

Target::Target(QObject *parent) :
    QObject(parent)
{}

void Target::storeInstant(int frame, const QPointF &p, const QPointF &s, const QPointF &a)
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
