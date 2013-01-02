#include "lineartrajectoriesproxymodel.hpp"

LinearTrajectoriesProxyModel::LinearTrajectoriesProxyModel(QObject *parent) :
    BaseProxyModel(parent)
{
    setColumn(VideoModel::LinearTrajectoriesColumn);
}

QVariant LinearTrajectoriesProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
}

QVariant LinearTrajectoriesProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
}

QModelIndex LinearTrajectoriesProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
}

QModelIndex LinearTrajectoriesProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
}

QModelIndex LinearTrajectoriesProxyModel::index(int row, int column, const QModelIndex &parent) const
{
}

QModelIndex LinearTrajectoriesProxyModel::parent(const QModelIndex &child) const
{
}

int LinearTrajectoriesProxyModel::rowCount(const QModelIndex &) const
{
}

int LinearTrajectoriesProxyModel::columnCount(const QModelIndex &) const
{
}
