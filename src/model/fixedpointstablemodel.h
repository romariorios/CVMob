#ifndef FIXEDPOINTSTABLEMODEL_H
#define FIXEDPOINTSTABLEMODEL_H

#include <QAbstractTableModel>

class FixedPointsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit FixedPointsTableModel(QObject *parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

};

#endif // FIXEDPOINTSTABLEMODEL_H
