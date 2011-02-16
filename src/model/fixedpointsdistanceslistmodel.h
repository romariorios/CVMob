#ifndef FIXEDPOINTSTABLEMODEL_H
#define FIXEDPOINTSTABLEMODEL_H

#include <QAbstractListModel>

#include <opencv/cv.h>

class FixedPointsDistancesListModel : public QAbstractListModel
{
    Q_OBJECT

    double calcDistance(CvPoint2D32f point1, CvPoint2D32f point2, float horizontalRatio, float verticalRatio) const;

public:
    explicit FixedPointsDistancesListModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
};

#endif // FIXEDPOINTSTABLEMODEL_H
