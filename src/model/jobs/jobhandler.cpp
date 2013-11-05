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

#include "jobhandler.hpp"

#include <model/jobs/basejob.hpp>

void JobHandler::startJob(BaseJob* j)
{
    _progress[j] = Progress();
    connect(j, &BaseJob::progressRangeChanged,
            this, &JobHandler::onJobRangeChanged, Qt::QueuedConnection);
    connect(j, &BaseJob::progressChanged,
            this, &JobHandler::onJobProgressChanged, Qt::QueuedConnection);
    connect(j, &QThread::finished,
            this, &JobHandler::onJobFinished, Qt::QueuedConnection);
    connect(j, &BaseJob::frameRequested,
            this, &JobHandler::onFrameRequested, Qt::QueuedConnection);
    
    if (_progress.size() == 1 || j->startFrame() < _currentFrameAvailable) {
        _currentFrameAvailable = j->startFrame();
        _currentLateJob = j;
    }
    
    emit jobAmountChanged(jobAmount());
    
    j->start();
}

void JobHandler::onVideoFrameChanged(int frame)
{
    _currentFrameAvailable = frame;
    
    for (BaseJob *job : _frameWantedBy[frame]) {
        job->onFrameReady(frame);
    }
}

void JobHandler::onVideoPlaybackChanged(bool playing)
{
    _videoPlaying = playing;
}

JobHandler::JobHandler(QObject* parent) :
    QObject(parent),
    _maximum(0),
    _curProgress(0)
{}

void JobHandler::onJobRangeChanged(int, int maximum)
{
    BaseJob *j = qobject_cast<BaseJob *>(sender());
    
    int oldMaximum = _progress[j].maximum;
    _progress[j].maximum = maximum;
    
    _maximum += maximum - oldMaximum;
    emit rangeChanged(0, _maximum);
}

void JobHandler::onJobProgressChanged(int progress)
{
    BaseJob *j = qobject_cast<BaseJob *>(sender());
    
    int oldProgress = _progress[j].value;
    _progress[j].value = progress;
    
    if (!_videoPlaying && j == _currentLateJob) { 
        _currentFrameAvailable = j->startFrame() + progress + 1;
        QSet<BaseJob *> &wantThisFrame = _frameWantedBy[_currentFrameAvailable];
        
        for (BaseJob *job : wantThisFrame) {
            job->onFrameReady(_currentFrameAvailable);
        }
        
        wantThisFrame = QSet<BaseJob *>();
    }
    
    _curProgress += progress - oldProgress;
    emit progressChanged(_curProgress);
}

void JobHandler::onJobFinished()
{
    BaseJob *j = qobject_cast<BaseJob *>(sender());
    
    _maximum -= _progress[j].maximum;
    _curProgress -= _progress[j].maximum;
    
    emit rangeChanged(0, _maximum);
    emit progressChanged(_curProgress);
    
    _progress.remove(j);
    if (_progress.empty()) {
        emit allFinished();
    }
    
    for (QSet<BaseJob *> &jobsWantingFrame : _frameWantedBy) {
        jobsWantingFrame.remove(j);
    }
    
    emit jobAmountChanged(jobAmount());
    
    j->deleteLater();
}

void JobHandler::onFrameRequested(int frame)
{
    BaseJob *j = qobject_cast<BaseJob *>(sender());
    
    if (frame > _currentFrameAvailable) {
        _frameWantedBy[frame].insert(j);
    } else {
        j->onFrameReady(frame);
    }
}
