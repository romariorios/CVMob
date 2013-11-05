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

#ifndef JOBHANDLER_H
#define JOBHANDLER_H

#include <QObject>

#include <QHash>
#include <QSet>

class BaseJob;

class JobHandler : public QObject
{
    Q_OBJECT
public:
    explicit JobHandler(QObject* parent);
    
    void startJob(BaseJob* j);
    inline int jobAmount() const { return _progress.size(); }
    
signals:
    void rangeChanged(int minimum, int maximum);
    void progressChanged(int progress);
    void allFinished();
    void jobAmountChanged(int amount);
    void frameReady(int frame);
    
private:
    void onJobRangeChanged(int, int maximum);
    void onJobProgressChanged(int progress);
    void onJobFinished();
    void onFrameRequested(int frame);
    
    struct Progress {
        Progress(int m = 0, int v = 0) : maximum(m), value(v) {}
        int maximum;
        int value;
    };
    QHash<BaseJob *, Progress> _progress;
    QHash<int, QSet<BaseJob *> > _frameWantedBy;
    int _maximum;
    int _curProgress;
    int _currentFrameAvailable;
    BaseJob *_currentLateJob;
};


#endif // JOBHANDLER_H
