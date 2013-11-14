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

#include <QObject>

#include <QAbstractItemModel>
#include <QVector>
#include <QPointF>
#include <QSize>

class BaseTarget : public QObject
{
protected:
    explicit BaseTarget(QObject* parent = 0);
    
    QModelIndex parentIndex;
    QAbstractItemModel *model;
    
    friend class BaseJob;
};

class JobHandler;

class BaseJob : public QObject
{
    Q_OBJECT
public:
    // TODO bring back individual search window size
    explicit BaseJob(const QVector<QPointF> &startPoints, int startFrame, int endFrame,
                     int videoRow, QAbstractItemModel *parent);
    void setTarget(const QModelIndex &index);
    virtual BaseTarget &target() = 0;
    
protected:
    void run();
    virtual void emitNewPoints(int frame,
                               const QVector<QPointF> &points) = 0;
    
private:
    QVector<QPointF> _currentPoints;
    int _startFrame;
    int _endFrame;
    int _videoRow;
    int _currentFrame;
    QAbstractItemModel *_model;
    
    friend class JobHandler;
};

#endif
