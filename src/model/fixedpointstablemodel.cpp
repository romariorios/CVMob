#include "fixedpointstablemodel.h"

FixedPointsTableModel::FixedPointsTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

QVariant FixedPointsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || section < 0 || section >= 2) {
        return QVariant();
    }

    switch (section) {
    case 0:
        return "Point";
    case 1:
        return "Distance";
    }
}
