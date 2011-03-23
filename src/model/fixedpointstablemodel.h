#ifndef FIXEDPOINTSTABLEMODEL_H
#define FIXEDPOINTSTABLEMODEL_H

#include <QAbstractListModel>

#include <QPoint>

class FixedPointsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit FixedPointsTableModel(QObject *parent = 0);

    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
};

#endif // FIXEDPOINTSTABLEMODEL_H
