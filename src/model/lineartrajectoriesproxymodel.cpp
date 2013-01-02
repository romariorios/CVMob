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
    if (sourceIndex.parent() == _parentIndex) {
        return index(sourceIndex.row(), 0);
    }

    if (sourceIndex.parent().parent() == _parentIndex) {
        return index(sourceIndex.row(), sourceIndex.column(), index(sourceIndex.parent().row(), 0));
    }
}

QModelIndex LinearTrajectoriesProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!_parentIndex.isValid() ||
        !proxyIndex.isValid()) {
        return QModelIndex();
    }

    if (!proxyIndex.parent().isValid()) {
        return sourceModel()->index(proxyIndex.row(), 0, _parentIndex);
    }

    QModelIndex parent = sourceModel()->index(proxyIndex.parent().row(), 0, _parentIndex);
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column(), parent);
}

QModelIndex LinearTrajectoriesProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return createIndex(row, column, parent.row());
    }

    return createIndex(row, column, -1);
}

QModelIndex LinearTrajectoriesProxyModel::parent(const QModelIndex &child) const
{
    if (child.internalId() == -1) {
        return QModelIndex();
    }

    return index(child.internalId(), 0);
}

int LinearTrajectoriesProxyModel::rowCount(const QModelIndex &parent) const
{
}

int LinearTrajectoriesProxyModel::columnCount(const QModelIndex &parent) const
{
}
