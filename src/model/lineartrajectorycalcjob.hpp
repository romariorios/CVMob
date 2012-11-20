#ifndef LINEARTRAJECTORYCALCJOB_HPP
#define LINEARTRAJECTORYCALCJOB_HPP

#include <QThread>

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

    friend class LinearTrajectoryCalcJob;
};

class LinearTrajectoryCalcJob : public QThread
{
    Q_OBJECT
public:
    explicit LinearTrajectoryCalcJob(const QPointF &startPoint,
                                     int videoRow,
                                     const QSize &windowSize,
                                     QAbstractItemModel *parent);
    void setTarget(const QModelIndex &targetIndex);

protected:
    void run();

signals:
    void instantGenerated(int frame, const QPointF &p, const QPointF &s, const QPointF &a);
    void rangeChanged(int minimum, int maximum);
    void progressChanged(int progress);

private:
    QPointF _startPoint;
    int _videoRow;
    int _framesCount;
    QSize _windowSize;
    QAbstractItemModel *_model;
    QModelIndex _framesParentIndex;
    Target _target;
};

#endif // LINEARTRAJECTORYCALCJOB_HPP
