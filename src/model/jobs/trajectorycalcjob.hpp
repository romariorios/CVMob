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

#ifndef TRAJECTORYCALCJOB_HPP
#define TRAJECTORYCALCJOB_HPP

#include <model/jobs/basejob.hpp>

#include <QAbstractItemModel>
#include <QPointF>
#include <QSize>

class TargetTrajectory : public BaseTarget {
    Q_OBJECT
private:
    TargetTrajectory(QObject *parent = 0);

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
    
protected:
    void emitNewPoints(int frame, const QVector< QPointF >& points);
    inline BaseTarget &target() { return _target; }

signals:
    void instantGenerated(int frame, const QPointF &p, const QPointF &s, const QPointF &a);

private:
    QPointF _previousPoint;
    QPointF _previousSpeed;
    TargetTrajectory _target;
};

#endif // TRAJECTORYCALCJOB_HPP
