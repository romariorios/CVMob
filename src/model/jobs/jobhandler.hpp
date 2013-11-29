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
    void startJob(BaseJob *j);
    void setWindowSize(const QSize &windowSize);
    
protected:
    void run();

signals:
    void rangeChanged(int minimum, int maximum);
    void progressChanged(int progress);
    void jobAmountChanged(int amount);

private:
    QVector<BaseJob *> _newJobs;
    QSize _windowSize;
    QMutex _lock;
    VideoModel *_model;
    const int _videoRow;
};

#endif // JOBHANDLER_H
