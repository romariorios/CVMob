/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

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

#include <model/videomodel.hpp>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QImage>
#include <QtGui/QResizeEvent>
#include <QtGui/QVBoxLayout>
#include <view/playbar.hpp>
#include <view/videographicsview.hpp>
#include <view/videostatus.hpp>

#include <QtGui/QLabel>

#include <QTimer>

#include <QDebug>
#include <cstdlib>
#include <ctime>

VideoView::VideoView(QWidget *parent) :
    QAbstractItemView(parent),
    _view(new VideoGraphicsView),
    _noVideoVideo(Video(new QGraphicsScene(_view), 0)),
    _playBar(new PlayBar(this)),
    _status(new VideoStatus(this))
{
    new QVBoxLayout(viewport());
    viewport()->layout()->addWidget(_status);
    _status->hide();
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

    connect(_playBar, SIGNAL(frameChanged(int)), SLOT(changeFrame(int)));
    connect(_playBar, SIGNAL(newDistanceRequested()), SLOT(beginDistanceCreation()));
    connect(_playBar, SIGNAL(newTrajectoryRequested()), SLOT(beginLTrajectoryCalculation()));
}

VideoView::~VideoView()
{
    delete _view;
}

void VideoView::showMessage(const QString &message, int duration)
{
    _status->show();
    _status->setMessage(message);
    if (duration) {
        QTimer::singleShot(duration, _status, SLOT(hide()));
    }
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
        Video v = _videos.at(topLeft.row());

        if (topLeft.column() == VideoModel::FrameSizeColumn) {
            QRectF r = QRectF(QPointF(0, 0),
                              topLeft.data(VideoModel::VideoSceneRole)
                              .toSizeF());
            v.bgRect->setRect(r);
            v.scene->setSceneRect(r);
        } else if (topLeft.column() == VideoModel::CurrentFrameColumn) {
            v.bgRect->setBrush(model()
                               ->index(topLeft
                                       .data(VideoModel::VideoSceneRole)
                                       .toInt(),
                                       0,
                                       model()
                                       ->index(topLeft
                                               .row(),
                                               VideoModel::FramesColumn))
                               .data(VideoModel::VideoSceneRole).value<QImage>());
        }
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.column() == VideoModel::DistancesColumn) {
            QGraphicsLineItem *line = _videos.at(parent.row()).distances.at(topLeft.row());
            line->setLine(topLeft.data(VideoModel::VideoSceneRole).toLineF());
        }
    }
}

void VideoView::selectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    QModelIndex selectedIndex = selected.at(0).indexes().at(0);
    _currentVideoRow = selectedIndex.row();

    _view->setScene(_videos.at(_currentVideoRow).scene);
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);

    _playBar->setPlayData(model()->rowCount(
                              model()->index(
                                  _currentVideoRow,
                                  VideoModel::FramesColumn)),
                          model()->data(
                              model()->index(_currentVideoRow,
                                             VideoModel::FrameDurationColumn)).toInt());
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
            QModelIndex index = model()->index(i, VideoModel::DistancesColumn);
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
        if (parent.column() == VideoModel::DistancesColumn) {
            for (int i = start; i <= end; ++i) {
                Video &v = _videos[parent.row()];
                v.distances << new QGraphicsLineItem(0, 0, 0, 0, v.bgRect, v.scene);
            }
        }
    }
}

void VideoView::changeFrame(int frame)
{
    QModelIndex currentFrameIndex = model()
            ->index(_currentVideoRow, VideoModel::CurrentFrameColumn);

    if (frame == currentFrameIndex.data(VideoModel::VideoSceneRole).toInt()) {
        return;
    }

    model()->setData(currentFrameIndex, frame, VideoModel::VideoSceneEditRole);
}

void VideoView::beginDistanceCreation()
{
    connect(_view, SIGNAL(mousePressed(QPointF)), SLOT(distanceFirstPoint(QPointF)));
    _status->setMessage(tr("Click and drag to measure a distance"));
}

void VideoView::beginLTrajectoryCalculation()
{
    _status->setMessage(tr("Click a point to track"));

    connect(_view, SIGNAL(mouseReleased(QPointF)), SLOT(calculateLTrajectoryFromPoint(QPointF)));
}

void VideoView::distanceFirstPoint(const QPointF &p)
{
    disconnect(_view, SIGNAL(mousePressed(QPointF)), this, SLOT(distanceFirstPoint(QPointF)));

    Video &currentVideo = _videos[_currentVideoRow];

    QGraphicsLineItem *guideLine =
            new QGraphicsLineItem(QLineF(p, p), currentVideo.bgRect, currentVideo.scene);
    guideLine->setPen(QColor(0, 0, 255));
    _videos[_currentVideoRow].distances << guideLine;

    connect(_view, SIGNAL(mouseDragged(QPointF)), SLOT(distanceUpdateSecondPoint(QPointF)));
    connect(_view, SIGNAL(mouseReleased(QPointF)), SLOT(distanceEndCreation(QPointF)));
}

void VideoView::distanceUpdateSecondPoint(const QPointF &p)
{
    QGraphicsLineItem *guideLine = _videos[_currentVideoRow].distances.last();
    guideLine->setLine(QLineF(guideLine->line().p1(), p));
    _status->setMessage(QString("(%1, %2)").arg(p.x()).arg(p.y()));
}

void VideoView::distanceEndCreation(const QPointF &p)
{
    QGraphicsLineItem *guideLine = _videos[_currentVideoRow].distances.takeLast();

    static_cast<VideoModel *>(model())->createDistance(guideLine->line(), _currentVideoRow);
    delete guideLine;

    _status->setMessage(tr("Done"));
    QTimer::singleShot(3000, _status, SLOT(hide()));

    disconnect(_view, SIGNAL(mouseDragged(QPointF)), this, SLOT(distanceUpdateSecondPoint(QPointF)));
    disconnect(_view, SIGNAL(mouseReleased(QPointF)), this, SLOT(distanceEndCreation(QPointF)));
}

void VideoView::calculateLTrajectoryFromPoint(const QPointF &p)
{
    int frame = model()->index(_currentVideoRow, VideoModel::CurrentFrameColumn)
            .data(VideoModel::VideoSceneRole).toInt();

    LinearTrajectoryCalcJob *job = static_cast<VideoModel *>(model())->calculateLinearTrajectory
            (p, frame, _currentVideoRow);
    _status->setJob(tr("Calculating trajectory..."), job);
    connect(job, SIGNAL(finished()), job, SLOT(deleteLater()));
    job->start();
}
