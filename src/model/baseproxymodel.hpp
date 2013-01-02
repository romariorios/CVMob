#ifndef BASEPROXYMODEL_HPP
#define BASEPROXYMODEL_HPP

#include <QAbstractProxyModel>

#include <model/videomodel.hpp>

class QItemSelectionModel;

class BaseProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit BaseProxyModel(QObject *parent = 0);

    void setSourceModel(QAbstractItemModel *sourceModel);
    void setSelectionModel(QItemSelectionModel *selectionModel);

protected:
    void setColumn(int column);

    QModelIndex _parentIndex;

private:
    int _column;

private slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void forwardDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void forwardRowInsertion(const QModelIndex &parent, int start, int end);
};

#endif // BASEPROXYMODEL_HPP
