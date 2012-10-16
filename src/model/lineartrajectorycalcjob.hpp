#ifndef LINEARTRAJECTORYCALCJOB_HPP
#define LINEARTRAJECTORYCALCJOB_HPP

#include <QObject>

#include <QPointF>

class LinearTrajectoryCalcJob : public QObject
{
    Q_OBJECT
public:
    explicit LinearTrajectoryCalcJob(const QPointF &startPoint, QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // LINEARTRAJECTORYCALCJOB_HPP
