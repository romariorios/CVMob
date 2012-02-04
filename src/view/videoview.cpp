#include "videoview.h"

#include <model/cvmobvideomodel.hpp>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QResizeEvent>
#include <QtGui/QVBoxLayout>

#include <QDebug>

VideoView::VideoView(QWidget *parent) :
    QAbstractItemView(parent),
    QGraphicsView(new QGraphicsScene, parent)
{}

QRect VideoView::visualRect(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QRect();
    }

    return QRect(index.model()->index(index.row(), 0).data().toPoint(),
                 index.model()->index(index.row(), 1).data().toPoint());
}

void VideoView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    CvmobVideoModel *model = static_cast<CvmobVideoModel *>(const_cast<QAbstractItemModel *>(topLeft.model()));

    for (int i = topLeft.row(); i < bottomRight.row(); ++i) {
        QGraphicsLineItem *line = new QGraphicsLineItem(
                    QLineF(model->data(model->index(0, 0, model->index(0, 4))).toPointF(),
                           model->data(model->index(0, 1, model->index(0, 4))).toPointF()));

        scene()->addItem(line);
    }
}

void VideoView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(index)
    Q_UNUSED(hint)
}

QModelIndex VideoView::indexAt(const QPoint &point) const
{
    foreach (QGraphicsItem *item, scene()->items(point, Qt::ContainsItemShape, Qt::AscendingOrder)) {
        for (int i = 0; i < model()->rowCount(); ++i) {
            QModelIndex index = model()->index(i, 0);
            if (index.data().toPointF() == item->boundingRect().topLeft()) {
                return index;
            }
        }
    }

    return QModelIndex();
}

QModelIndex VideoView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(cursorAction)
    Q_UNUSED(modifiers)

    return QModelIndex();
}

int VideoView::horizontalOffset() const
{
    return 0;
}

int VideoView::verticalOffset() const
{
    return 0;
}

bool VideoView::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index)

    return true;
}

void VideoView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    Q_UNUSED(rect)
    Q_UNUSED(command)
}

QRegion VideoView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_UNUSED(selection)

    return QRegion();
}

void VideoView::resizeEvent(QResizeEvent *event)
{
    QSizeF change = event->size() - event->oldSize();

    if (change.width() > 4*change.height()/3) {
        change.setWidth(4*change.height()/3);
    } else {
        change.setHeight(3*change.width()/4);
    }

    QSizeF sceneSize = scene()->sceneRect().size() + change;
    QRectF sceneRect = scene()->sceneRect();
    sceneRect.setSize(sceneSize);

    qDebug() << event->size();
    qDebug() << event->oldSize();
    qDebug() << sceneRect;

    scene()->setSceneRect(sceneRect);
}