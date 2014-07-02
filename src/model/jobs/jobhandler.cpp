/*
    CVMob - Motion capture program
    Copyright (C) 2013, 2014  The CVMob contributors

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

#include "jobhandler.hpp"

#include <model/videomodel.hpp>
#include <model/jobs/basejob.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include <QImage>
#include <QMutexLocker>
#include <QSize>
#include <QTimer>
#include <QTimerEvent>

#include <vector>

#include <QDebug>

JobHandler::JobHandler(int videoRow, VideoModel* parent) :
    QThread(parent),
    _model(parent),
    _videoRow(videoRow),
    _videoFrame(-1),
    _playStatus(false),
    _previousProgress(-1)
{}

JobHandler::~JobHandler()
{
    quit();
}

void JobHandler::startJob(BaseJob* j)
{
    QMutexLocker l(&_lock);

    if (_stopRequested) {
        return;
    }

    _newJobs.append(j);
    j->emitNewPoints(j->_startFrame, j->_currentPoints);

    l.unlock();

    if (!isRunning()) {
        start();
    }
}

static QVector< QPointF > trackPoints(const QVector< QPointF >& startPoints,
                                      const QImage& startFrame,
                                      const QImage& endFrame,
                                      const QSize &windowSize)
{
    if (startPoints.isEmpty()) {
        return QVector<QPointF>();
    }

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

    cvWindowSize.height = windowSize.height();
    cvWindowSize.width = windowSize.width();

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

void JobHandler::run()
{
    QVector<BaseJob *> jobs;
    int previousFrame = -2;
    int currentFrame = -1;
    int maximumProgress = 0;
    int curProgress = 0;
    QImage previousFrameImage;

    _lock.lock();
    QSize winSize = _windowSize;
    _lock.unlock();

    startProgressTimer();

    do {
        _lock.lock();
        int videoFrame = _videoFrame;
        bool playing = _playStatus;

        if (_stopRequested) {
            _newJobs.clear();

            for (auto j : jobs) {
                j->deleteLater();
            }
            jobs.clear();
        }
        _lock.unlock();

        if (playing && videoFrame < currentFrame) {
            yieldCurrentThread();
            continue;
        }

        bool jobsWereAdded = false;
        while (!_newJobs.empty()) {
            _lock.lock();
            BaseJob *j = _newJobs.takeFirst();
            _lock.unlock();

            int jobFrame = j->_startFrame;
            currentFrame = currentFrame < 0 || currentFrame > jobFrame?
                jobFrame : currentFrame;
            maximumProgress += j->_endFrame - j->_startFrame;
            jobs.append(j);

            if (!jobsWereAdded) {
                jobsWereAdded = true;
            }
        }

        if (jobsWereAdded) {
            emit jobAmountChanged(jobs.size());
            emit rangeChanged(0, maximumProgress);
        }

        _progressTimerLock.lock();
        _progress = curProgress;
        _progressTimerLock.unlock();

        _lock.lock();
        QModelIndex currentFrameIndex = _model->index({
            { _videoRow, VideoModel::AllFramesCol },
            { currentFrame, 0 }
        });
        _lock.unlock();

        QImage currentFrameImage = _model->data(currentFrameIndex).value<QImage>();

        if (currentFrame != previousFrame + 1) {
            previousFrame = currentFrame;
            previousFrameImage = currentFrameImage;
            ++currentFrame;
            continue;
        }

        QVector<QPointF> previousPoints;

        for (BaseJob *j : jobs) {
            if (j->_currentFrame != previousFrame) {
                continue;
            }

            previousPoints += j->_currentPoints;
        }

        QVector<QPointF> currentPoints = trackPoints(previousPoints, previousFrameImage,
                                                     currentFrameImage, winSize);

        bool jobsWereRemoved = false;
        for (int i = 0; i < jobs.size(); ++i) {
            BaseJob *j = jobs.at(i);

            // This workaround is currently necessary because some jobs never get deleted for
            // some reason (their currentFrame becomes less than previousFrame, thus they will
            // never catch up with the rest of the jobs again), making the thread run forever.
            // TODO Find out why that happens, fix it and remove this hack
            if (j->_currentFrame < previousFrame) {
                jobs.remove(i);
                j->deleteLater();

                if (!jobsWereRemoved) {
                    jobsWereRemoved = true;
                }
            }

            if (j->_currentFrame != previousFrame) {
                continue;
            }

            for (QPointF &p : j->_currentPoints) {
                p = currentPoints.takeFirst();
            }

            j->emitNewPoints(currentFrame, j->_currentPoints);

            ++j->_currentFrame;
            if (j->_currentFrame >= j->_endFrame) {
                jobs.remove(i);
                j->deleteLater();

                if (!jobsWereRemoved) {
                    jobsWereRemoved = true;
                }
            }

            ++curProgress;
        }

        if (jobsWereRemoved) {
            emit jobAmountChanged(jobs.size());
        }

        previousFrame = currentFrame;
        previousFrameImage = currentFrameImage;
        ++currentFrame;
    } while (!jobs.empty());

    stopProgressTimer();

    _lock.lock();
    _stopRequested = false;
    _lock.unlock();
}

void JobHandler::timerEvent(QTimerEvent* e)
{
    _progressTimerLock.lock();
    if (e->timerId() == _progressTimerId && _previousProgress != _progress) {
        _previousProgress = _progress;
        emit progressChanged(_progress);
    }
    _progressTimerLock.unlock();

    QObject::timerEvent(e);
}

void JobHandler::startProgressTimer()
{
    _progressTimerId = startTimer(100);
}

void JobHandler::stopProgressTimer()
{
    killTimer(_progressTimerId);
}

void JobHandler::setWindowSize(const QSize& windowSize)
{
    _lock.lock();
    _windowSize = windowSize;
    _lock.unlock();
}

void JobHandler::setVideoFrame(int frame)
{
    _lock.lock();
    _videoFrame = frame;
    _lock.unlock();
}

void JobHandler::setPlayStatus(bool playStatus)
{
    _lock.lock();
    _playStatus = playStatus;
    _lock.unlock();
}

void JobHandler::stopAll()
{
    _lock.lock();
    _stopRequested = true;
    _lock.unlock();
}
