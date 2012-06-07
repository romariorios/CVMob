#ifndef DISTANCESPROXYMODEL_HPP
#define DISTANCESPROXYMODEL_HPP

#include <QIdentityProxyModel>

class QItemSelectionModel;

class DistancesProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit DistancesProxyModel(QObject *parent = 0);

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & = QModelIndex()) const;
    int columnCount(const QModelIndex & = QModelIndex()) const;
    void setSelectionModel(QItemSelectionModel *selectionModel);

private:
    QModelIndex _parentIndex;

private slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif // DISTANCESPROXYMODEL_HPP
