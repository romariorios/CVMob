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
    _viewport(new QGraphicsView)
{
    setViewport(_viewport);

    _viewport->setScene(new QGraphicsScene(fitRectWithProportion(_viewport->rect(), 4./3),
                                           _viewport));
    _viewport->scene()->addLine(0, 0, 100, 100);
}

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
                    QLineF(model->data(model->index(0, 0, model->index(0, 0))).toPointF(),
                           model->data(model->index(0, 1, model->index(0, 0))).toPointF()));

        _viewport->scene()->addItem(line);
    }
}

void VideoView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(index)
    Q_UNUSED(hint)
}

QModelIndex VideoView::indexAt(const QPoint &point) const
{
    foreach (QGraphicsItem *item, _viewport->scene()->items(point, Qt::ContainsItemShape, Qt::AscendingOrder)) {
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

void VideoView::resizeEvent(QResizeEvent *)
{
    _viewport->scene()->setSceneRect(fitRectWithProportion(_viewport->rect(), 4./3));
}


const QRectF VideoView::fitRectWithProportion(const QRectF &rect, qreal proportion) const
{
    if (rect.width() > rect.height() * proportion) {
        qreal newWidth = rect.height() * proportion;
        qreal widthDiff = rect.width() - newWidth;
        return QRectF(rect.left() + widthDiff / 2, rect.top(), newWidth, rect.height());
    }

    if (rect.width() < rect.height() * proportion) {
        qreal newHeight = rect.width() * (1 / proportion);
        qreal heightDiff = rect.height() - newHeight;
        return QRectF(rect.left(), rect.top() + heightDiff / 2, rect.width(), newHeight);
    }

    return rect;
}
