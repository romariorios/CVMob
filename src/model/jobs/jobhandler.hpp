/*
    CVMob - Motion capture program
    Copyright (C) 2013--2015  The CVMob contributors

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

#ifndef JOBHANDLER_H
#define JOBHANDLER_H

#include <QObject>

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QVector>

class BaseJob;
class VideoModel;

class JobHandler : public QThread
{
    Q_OBJECT
public:
    explicit JobHandler(int videoRow, VideoModel* parent);
    ~JobHandler();
    void startJob(BaseJob *j);
    void setWindowSize(const QSize &windowSize);
    void setVideoFrame(int frame);
    void setPlayStatus(bool playStatus);
    void removeJob(BaseJob *j);

public slots:
    void stopAll();

protected:
    void run();
    void timerEvent(QTimerEvent *e);

signals:
    void rangeChanged(int minimum, int maximum);
    void progressChanged(int progress);
    void jobAmountChanged(int amount);

private:
    void startProgressTimer();
    void stopProgressTimer();

    QVector<BaseJob *> _newJobs;
    QVector<BaseJob *> _jobsToRemove;
    QSize _windowSize;
    QMutex _lock;
    VideoModel *_model;
    const int _videoRow;
    int _videoFrame;
    bool _playStatus;
    int _progressTimerId;
    QMutex _progressTimerLock;
    int _progress;
    int _previousProgress;
    bool _stopRequested = false;
};

#endif // JOBHANDLER_H
