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

#ifndef BASEJOB_HPP
#define BASEJOB_HPP

#include <QThread>

#include <QAbstractItemModel>
#include <QVector>
#include <QPointF>
#include <QSize>
#include <QMutex>
#include <QWaitCondition>

class BaseTarget : public QObject
{
protected:
    explicit BaseTarget(QObject* parent = 0);
    
    QModelIndex parentIndex;
    QAbstractItemModel *model;
    
    friend class BaseJob;
};

class BaseJob : public QThread
{
    Q_OBJECT
public:
    explicit BaseJob(const QVector<QPointF> &startPoints, int startFrame, int endFrame,
                     int videoRow, const QSize &windowSize, QAbstractItemModel *parent);
    void onFrameReady(int frame);
    void setTarget(const QModelIndex &index);
    inline int startFrame() const { return _startFrame; }
    
protected:
    void run();
    virtual void emitNewPoints(int frame,
                               const QVector<QPointF> &points) = 0;
    virtual BaseTarget &target() = 0;

signals:
    void progressRangeChanged(int minimum, int maximum);
    void progressChanged(int progress);
    void frameRequested(int frame);
    
private:
    QVector<QPointF> trackPoints(const QVector<QPointF> &startPoints,
                                 const QImage &startFrame,
                                 const QImage &endFrame);
    void requestFrame(int frame);
    
    QVector<QPointF> _startPoints;
    int _startFrame;
    int _endFrame;
    int _videoRow;
    int _wantedFrame;
    QSize _windowSize;
    QAbstractItemModel *_model;
    QMutex _mutex;
    QWaitCondition _frameIsAvailable;
};

#endif
