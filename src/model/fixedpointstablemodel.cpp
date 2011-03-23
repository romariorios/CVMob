#include "fixedpointstablemodel.h"

#include "model/OpenCV/ProxyOpenCv.h"

using namespace model;

FixedPointsTableModel::FixedPointsTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

// TODO: Convert to QPoint.
double FixedPointsTableModel::calcDistance(CvPoint2D32f point1, CvPoint2D32f point2, float horizontalRatio, float verticalRatio) const
{
    double dx = point2.x - point1.x;
    double dy = point2.y - point1.y;
    return sqrt(dx * dx * horizontalRatio * horizontalRatio + dy * dy * verticalRatio * verticalRatio);
}

int FixedPointsTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 2;
}

QVariant FixedPointsTableModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole ||
        index.row() < 0 ||
        index.row() >= rowCount(QModelIndex()) ||
        index.column() < 0 ||
        index.column() >= columnCount(QModelIndex())) {
        return QVariant();
    }

    if (index.column() == 1) {
        ProxyOpenCv *cvInstance = ProxyOpenCv::getInstance();
        return calcDistance(cvInstance->fixedPoints.at(index.row() * 2).markedPoint,
                            cvInstance->fixedPoints.at(index.row() * 2 + 1).markedPoint,
                            cvInstance->get_horizontalRazao(),
                            cvInstance->get_verticalRazao());
    }

    return index.row() + 1;
}

QVariant FixedPointsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        return section;
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return "Point";
        case 1:
            return "Distance";
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int FixedPointsTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return ProxyOpenCv::getInstance()->fixedPoints.size() / 2;
}
