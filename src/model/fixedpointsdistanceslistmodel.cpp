#include "fixedpointsdistanceslistmodel.h"

#include "model/OpenCV/ProxyOpenCv.h"

using namespace model;

FixedPointsDistancesListModel::FixedPointsDistancesListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

// TODO: Convert to QPoint.
double FixedPointsDistancesListModel::calcDistance(CvPoint2D32f point1, CvPoint2D32f point2, float horizontalRatio, float verticalRatio) const
{
    double dx = point2.x - point1.x;
    double dy = point2.y - point1.y;
    return sqrt(dx * dx * horizontalRatio * horizontalRatio + dy * dy * verticalRatio * verticalRatio);
}

QVariant FixedPointsDistancesListModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole ||
        index.row() < 0 ||
        index.row() >= rowCount(QModelIndex())) {
        return QVariant();
    }

    ProxyOpenCv *cvInstance = ProxyOpenCv::getInstance();
    return calcDistance(cvInstance->fixedPoints.at(index.row() * 2).markedPoint,
                        cvInstance->fixedPoints.at(index.row() * 2 + 1).markedPoint,
                        cvInstance->get_horizontalRazao(),
                        cvInstance->get_verticalRazao());
}

QVariant FixedPointsDistancesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        return section;
    }

    if (orientation == Qt::Horizontal &&
        section == 0) {
        return "Distance";
    }

    return QVariant();
}

int FixedPointsDistancesListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return ProxyOpenCv::getInstance()->fixedPoints.size() / 2;
}
