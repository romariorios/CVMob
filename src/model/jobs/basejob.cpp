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

#include <model/videomodel.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include <QImage>
#include <QSize>

#include <vector>

BaseJob::BaseJob(const QVector< QPointF >& startPoints, int startFrame, int endFrame, int videoRow, const QSize& windowSize, QAbstractItemModel* parent) :
    QThread(parent),
    _startPoints(startPoints),
    _startFrame(startFrame),
    _endFrame(endFrame),
    _videoRow(videoRow),
    _windowSize(windowSize),
    _model(parent)
{}

void BaseJob::onFrameReady(int frame)
{
    _mutex.lock();
    
    if (frame == _wantedFrame) {
        _frameIsAvailable.wakeAll();
    }
    
    _mutex.unlock();
}

void BaseJob::run()
{
    QModelIndex framesParentIndex = _model->index(_videoRow, VideoModel::FramesColumn);

    emit progressRangeChanged(0, _endFrame - _startFrame);
    
    requestFrame(0);
    emitNewPoints(_startFrame, _startPoints);
    emit progressChanged(0);

    QVector<QPointF> previousPoints = _startPoints;

    for (int i = _startFrame + 1; i <= _endFrame; ++i) {
        requestFrame(i);
        
        QVector<QPointF> newPoints = trackPoints(
            previousPoints,
            _model->index(i - 1, 0, framesParentIndex).data().value<QImage>(),
            _model->index(i, 0, framesParentIndex).data().value<QImage>()
        );

        emitNewPoints(i, newPoints);
        emit progressChanged(i - _startFrame + 1);

        previousPoints = newPoints;
    }
}

QVector< QPointF > BaseJob::trackPoints(const QVector< QPointF >& startPoints,
                                        const QImage& startFrame,
                                        const QImage& endFrame)
{
    QImage i8startFrame(startFrame.convertToFormat(QImage::Format_Indexed8));
    QImage i8endFrame(endFrame.convertToFormat(QImage::Format_Indexed8));
    cv::Mat cvStartFrame(i8startFrame.height(), i8startFrame.width(), CV_8UC1, i8startFrame.scanLine(0));
    cv::Mat cvEndFrame(i8endFrame.height(), i8endFrame.width(), CV_8UC1, i8endFrame.scanLine(0));
    
    std::vector<cv::Point2f> cvStartPoints;
    
    for (const QPointF &startP : startPoints) {
        cv::Point2f cvP;
        cvP.x = startP.x();
        cvP.y = startP.y();
        cvStartPoints.push_back(cvP);
    }
    
    std::vector<cv::Point2f> cvEndPoints;
    std::vector<uchar> status;
    std::vector<float> err;
    cv::Size cvWindowSize;

    cvWindowSize.height = _windowSize.height();
    cvWindowSize.width = _windowSize.width();

    cv::calcOpticalFlowPyrLK(cvStartFrame, cvEndFrame, cvStartPoints, cvEndPoints, status, err,
                            cvWindowSize,
                            // Magic numbers:
                            3, cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS,
                                                20, 0.01),
                            0.5, 0);
    
    QVector<QPointF> endPoints;
    
    for (const cv::Point2f &cvEndP : cvEndPoints) {
        endPoints << QPointF(cvEndP.x, cvEndP.y);
    }
    
    return endPoints;
}

void BaseJob::requestFrame(int frame)
{
    _mutex.lock();
    
    _wantedFrame = frame;
    emit frameRequested(_wantedFrame);
    
    _frameIsAvailable.wait(&_mutex);
    
    _mutex.unlock();
}
