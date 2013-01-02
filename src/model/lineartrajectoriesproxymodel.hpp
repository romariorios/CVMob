#ifndef LINEARTRAJECTORIESPROXYMODEL_HPP
#define LINEARTRAJECTORIESPROXYMODEL_HPP

#include <model/baseproxymodel.hpp>

class LinearTrajectoriesProxyModel : public BaseProxyModel
{
    Q_OBJECT
public:
    explicit LinearTrajectoriesProxyModel(QObject *parent = 0);
    
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
};

#endif // LINEARTRAJECTORIESPROXYMODEL_HPP
