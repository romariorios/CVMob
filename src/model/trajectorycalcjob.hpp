/*
    CVMob - Motion capture program
    Copyright (C) 2012  The CVMob contributors

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

#ifndef TRAJECTORYCALCJOB_HPP
#define TRAJECTORYCALCJOB_HPP

#include <model/basejob.hpp>

#include <QAbstractItemModel>
#include <QPointF>
#include <QSize>

class Target : public QObject {
    Q_OBJECT
private:
    Target(QObject *parent = 0);

    QModelIndex parentIndex;
    QAbstractItemModel *model;

private slots:
    void storeInstant(int frame, const QPointF &p, const QPointF &s, const QPointF &a);

    friend class TrajectoryCalcJob;
};

class TrajectoryCalcJob : public BaseJob
{
    Q_OBJECT
public:
    explicit TrajectoryCalcJob(const QPointF &startPoint,
                                     int startFrame,
                                     int endFrame,
                                     int videoRow,
                                     const QSize &windowSize,
                                     QAbstractItemModel *parent);
    void setTarget(const QModelIndex &targetIndex);

protected:
    void run();

signals:
    void instantGenerated(int frame, const QPointF &p, const QPointF &s, const QPointF &a);

private:
    QPointF _startPoint;
    int _videoRow;
    int _startFrame;
    int _endFrame;
    QSize _windowSize;
    QAbstractItemModel *_model;
    QModelIndex _framesParentIndex;
    Target _target;
};

#endif // TRAJECTORYCALCJOB_HPP
