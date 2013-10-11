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

#ifndef ANGLECALCJOB_HPP
#define ANGLECALCJOB_HPP

#include <model/jobs/basejob.hpp>

class TargetAngle : public QObject
{
    Q_OBJECT
private:
    explicit TargetAngle(QObject* parent = 0);
    
    QModelIndex parentIndex;
    QAbstractItemModel *model;
    
private:
    void storeInstant(int frame, float aSpeed, float aAccel, const QPointF &centralEdge,
                      const QPointF &pEdge1, const QPointF &pEdge2);
    
    friend class AngleCalcJob;
};

class AngleCalcJob : public BaseJob
{
    Q_OBJECT
public:
    explicit AngleCalcJob(const QVector< QPointF >& startAngle, int startFrame, int endFrame,
                          int videoRow, const QSize& windowSize, QAbstractItemModel* parent);
    void setTarget(const QModelIndex &targetIndex);
    
protected:
    void emitNewPoints(int frame, const QVector<QPointF> &points);
    
signals:
    void instantGenerated(int frame, float aSpeed, float aAccel, const QPointF &centralEdge,
                          const QPointF &pEdge1, const QPointF &pEdge2);
    
private:
    TargetAngle _target;
    float _previousAngle;
    float _previousASpeed;
};

#endif
