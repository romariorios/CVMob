/*
    CVMob - Motion capture program
    Copyright (C) 2012  The CVMob contributors

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "videoview.h"

#include <model/cvmobvideomodel.hpp>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QImage>
#include <QtGui/QResizeEvent>
#include <QtGui/QVBoxLayout>
#include <view/playbar.hpp>

#include <QtGui/QLabel>

#include <QDebug>

VideoView::VideoView(QWidget *parent) :
    QAbstractItemView(parent),
    _view(new QGraphicsView),
    _noVideoVideo(Video(new QGraphicsScene(_view), 0)),
    _playBar(new PlayBar(this))
{
    new QVBoxLayout(viewport());
    viewport()->layout()->addWidget(_view);
    viewport()->layout()->setMargin(0);
    viewport()->layout()->setSpacing(0);
    _view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setRenderHint(QPainter::Antialiasing);
    viewport()->layout()->addWidget(_playBar);

    QImage bgImage(":/images/translucent-logo.png");
    _view->setScene(_noVideoVideo.scene);
    _noVideoVideo.bgRect = new QGraphicsRectItem(0, _noVideoVideo.scene);
    _noVideoVideo.bgRect->setRect(QRectF(QPointF(0, 0), bgImage.size()));
    _noVideoVideo.bgRect->setBrush(bgImage);
    _noVideoVideo.bgRect->setPen(Qt::NoPen);
    _noVideoVideo.scene->setSceneRect(QRectF(QPointF(0, 0), bgImage.size()));
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);
    QGraphicsItem *noVideoText = _noVideoVideo.scene->addText(tr("No video"));
    noVideoText->moveBy(100, 50);
}

VideoView::~VideoView()
{
    delete _view;
}

QRect VideoView::visualRect(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QRect();
    }

    return QRect(index.model()->index(index.row(), 0).data().toPoint(),
                 index.model()->index(index.row(), 1).data().toPoint());
}

void VideoView::dataChanged(const QModelIndex &topLeft, const QModelIndex &)
{
    const QModelIndex parent = topLeft.parent();

    if (!parent.isValid()) { // Level 0
        if (topLeft.column() == CvmobVideoModel::FrameSizeColumn) {
            Video v = _videos.at(topLeft.row());
            QRectF r = QRectF(QPointF(0, 0),
                              topLeft.data(CvmobVideoModel::VideoSceneRole)
                              .toSizeF());
            v.bgRect->setRect(r);
            v.scene->setSceneRect(r);
        }
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.column() == CvmobVideoModel::DistancesColumn) {
            QGraphicsLineItem *line = _videos.at(parent.row()).distances.at(topLeft.row());
            line->setLine(topLeft.data(CvmobVideoModel::VideoSceneRole).toLineF());
        }
    }
}

void VideoView::selectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    QModelIndex selectedIndex = selected.at(0).indexes().at(0);

    _view->setScene(_videos.at(selectedIndex.row()).scene);
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);

    _playBar->setPlayData(model()->columnCount(
                              model()->index(
                                  selectedIndex.row(),
                                  CvmobVideoModel::FramesColumn)),
                          model()->data(
                              model()->index(selectedIndex.row(),
                                             CvmobVideoModel::FrameDurationColumn)).toInt());
}

void VideoView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    Q_UNUSED(index)
    Q_UNUSED(hint)
}

QModelIndex VideoView::indexAt(const QPoint &point) const
{
    foreach (QGraphicsItem *item, _view->scene()->items(point, Qt::ContainsItemShape, Qt::AscendingOrder)) {
        for (int i = 0; i < model()->rowCount(); ++i) {
            QModelIndex index = model()->index(i, CvmobVideoModel::DistancesColumn);
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
{;
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);

    QAbstractItemView::resizeEvent(event);
}

void VideoView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    if (!parent.isValid()) { // Level 0
        for (int i = start; i <= end; ++i) {
            _videos.insert(i, Video(new QGraphicsScene(_view), 0));
            Video &v = _videos.last();
            v.bgRect = new QGraphicsRectItem(0, v.scene);
            v.bgRect->setPen(Qt::NoPen);
        }
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.column() == CvmobVideoModel::DistancesColumn) {
            for (int i = start; i <= end; ++i) {
                Video &v = _videos[parent.row()];
                v.distances << new QGraphicsLineItem(0, 0, 0, 0, v.bgRect, v.scene);
            }
        }
    }
}
