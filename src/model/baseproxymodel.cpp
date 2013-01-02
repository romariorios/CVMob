#include "baseproxymodel.hpp"

#include <QItemSelectionModel>
#include <QItemSelection>

BaseProxyModel::BaseProxyModel(QObject *parent) :
    QAbstractProxyModel(parent),
    _column(VideoModel::InvalidColumn)
{
}

QModelIndex BaseProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex BaseProxyModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

void BaseProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            SLOT(forwardDataChange(QModelIndex,QModelIndex)));
    connect(sourceModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
            SLOT(forwardRowInsertion(QModelIndex,int,int)));
}

void BaseProxyModel::setSelectionModel(QItemSelectionModel *selectionModel)
{
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

void BaseProxyModel::setColumn(int column)
{
    _column = column;
}

void BaseProxyModel::selectionChanged(const QItemSelection &selected,
                                      const QItemSelection &deselected)
{
    Q_UNUSED(deselected)

    beginResetModel();

    if (selected.size() != 1 ||
        selected.at(0).indexes().size() != 1 ||
        _column == VideoModel::InvalidColumn) {
        _parentIndex = QModelIndex();
    } else {
        _parentIndex = sourceModel()->index(selected.at(0).indexes().at(0).row(), _column);
    }

    endResetModel();
}

void BaseProxyModel::forwardDataChange(const QModelIndex &topLeft,
                                       const QModelIndex &bottomRight)
{
    beginResetModel();
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
    endResetModel();
}

void BaseProxyModel::forwardRowInsertion(const QModelIndex &parent, int start, int end)
{
    if (parent == _parentIndex) {
        beginResetModel();
        endResetModel();
    }
}
