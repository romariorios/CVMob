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

void JobHandler::addJob(BaseJob* j)
{
    _progress[j] = Progress();
    connect(j, &BaseJob::progressRangeChanged, [this, j](int min, int max){
        onJobRangeChanged(j, max);
    });
    connect(j, &BaseJob::progressChanged, [this, j](int progress){
        onJobProgressChanged(j, progress);
    });
    connect(j, &QThread::finished, [this, j](){
        onJobFinished(j);
    });
    connect(j, &BaseJob::frameRequested, this ,&JobHandler::onFrameRequested, Qt::QueuedConnection);
    
    emit jobAmountChanged(jobAmount());
}

JobHandler::JobHandler(QObject* parent) :
    QObject(parent),
    _maximum(0),
    _curProgress(0)
{}

void JobHandler::onJobRangeChanged(BaseJob* j, int maximum)
{
    int oldMaximum = _progress[j].maximum;
    _progress[j].maximum = maximum;
    
    _maximum += maximum - oldMaximum;
    emit rangeChanged(0, _maximum);
}

void JobHandler::onJobProgressChanged(BaseJob* j, int progress)
{
    int oldProgress = _progress[j].value;
    _progress[j].value = progress;
    
    _curProgress += progress - oldProgress;
    emit progressChanged(_curProgress);
}

void JobHandler::onJobFinished(BaseJob* j)
{
    _maximum -= _progress[j].maximum;
    _curProgress -= _progress[j].maximum;
    
    emit rangeChanged(0, _maximum);
    emit progressChanged(_curProgress);
    
    _progress.remove(j);
    if (_progress.empty()) {
        emit allFinished();
    }
    
    emit jobAmountChanged(jobAmount());
}

void JobHandler::onFrameRequested(int frame)
{
    BaseJob *j = qobject_cast<BaseJob *>(sender());
    
    j->onFrameReady(frame);
}
