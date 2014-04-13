/*
    CVMob - Motion capture program
    Copyright (C) 2013, 2014  The CVMob contributors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "videoview.hpp"

#include <model/jobs/jobhandler.hpp>
#include <model/videomodel.hpp>
#include <QDialog>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QImage>
#include <QLineF>
#include <QResizeEvent>
#include <QSettings>
#include <QVBoxLayout>
#include <view/graphicsitems/angleitem.hpp>
#include <view/graphicsitems/distanceitem.hpp>
#include <view/graphicsitems/trajectoryinstantitem.hpp>
#include <view/graphicsitems/trajectoryitem.hpp>
#include <view/playbar.hpp>
#include <view/videographicsview.hpp>
#include <view/videostatus.hpp>

#include "ui_scalecalibrationdialog.h"

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

    QSettings set;

    _view->setRenderHint(set.value("video/antiAlias", true).toBool()?
        QPainter::Antialiasing : QPainter::NonCosmeticDefaultPen);
    viewport()->layout()->addWidget(_playBar);

    QImage bgImage(":/images/translucent-logo.png");
    _view->setScene(_noVideoVideo.scene);
    _noVideoVideo.bgRect = new QGraphicsRectItem(0);
    _noVideoVideo.bgRect->setRect(QRectF(QPointF(0, 0), bgImage.size()));
    _noVideoVideo.bgRect->setBrush(bgImage);
    _noVideoVideo.bgRect->setPen(Qt::NoPen);
    _noVideoVideo.scene->addItem(_noVideoVideo.bgRect);
    _noVideoVideo.scene->setSceneRect(QRectF(QPointF(0, 0), bgImage.size()));
    QGraphicsItem *noVideoText = _noVideoVideo.scene->addText(tr("No video"));
    noVideoText->moveBy(100, 50);

    connect(_playBar, SIGNAL(newDistanceRequested()), SLOT(beginDistanceCreation()));
    connect(_playBar, SIGNAL(newAngleRequested()), SLOT(beginAngleCreation()));
    connect(_playBar, SIGNAL(scaleCalibrationRequested()), SLOT(beginScaleCalibration()));

    connect(_playBar, SIGNAL(settingsRequested()), SIGNAL(settingsRequested()));

    connect(_playBar, &PlayBar::frameChanged, [=](int frame)
    {
        QModelIndex currentFrameIndex = model()
                ->index(_currentVideoRow, VideoModel::CurrentFrameCol);

        if (frame == currentFrameIndex.data().toInt()) {
            return;
        }

        model()->setData(currentFrameIndex, frame);
    });

    connect(_playBar, &PlayBar::playingChanged, [=](bool isPlaying) {
        QModelIndex currentPlayStatusIndex =
            model()->index(_currentVideoRow, VideoModel::PlayStatusCol);

        if (isPlaying == currentPlayStatusIndex.data().toBool()) {
            return;
        }

        model()->setData(currentPlayStatusIndex, isPlaying);
    });

    connect(_playBar, &PlayBar::newTrajectoryRequested, [=]()
    {
        auto status = new Status::Persistent(_status, tr("Click a point to track"));

        connect(_view, SIGNAL(mouseReleased(QPointF)), SLOT(calculateTrajectory(QPointF)));
        connect(_view, SIGNAL(mouseReleased(QPointF)), status, SLOT(deleteLater()));
    });
}

VideoView::~VideoView()
{
    delete _view;
}

void VideoView::showMessage(const QString &message, int duration)
{
    new Status::Message(_status, message, duration);
}

QRect VideoView::visualRect(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QRect();
    }

    return QRect(index.model()->index(index.row(), 0).data().toPoint(),
                 index.model()->index(index.row(), 1).data().toPoint());
}

void VideoView::updateSettings()
{
    QSettings set;

    _view->setRenderHint(set.value("video/antiAlias", true).toBool()?
        QPainter::Antialiasing : QPainter::NonCosmeticDefaultPen);
    _playBar->updateSettings();
}

void VideoView::dataChanged(const QModelIndex &topLeft, const QModelIndex &, const QVector<int> &)
{
    const QModelIndex parent = topLeft.parent();

    if (!parent.isValid()) { // Level 0
        Video v = _videos.at(topLeft.row());

        if (topLeft.column() == VideoModel::FrameSizeCol) {
            QRectF r = QRectF(QPointF(0, 0),
                              topLeft.data().toSizeF());
            v.bgRect->setRect(r);
            v.scene->setSceneRect(r);
        } else if (topLeft.column() == VideoModel::CurrentFrameCol) {
            int frame = topLeft.data().toInt();

            v.bgRect->setBrush(videoModel()->index({
                { topLeft.row(), VideoModel::AllFramesCol },
                { frame, 0 }
            }).data().value<QImage>());

            for (TrajectoryItem *traj : v.trajectories) {
                traj->setCurrentFrame(frame);
            }

            for (int i = 0; i < v.angles.size(); ++i) {
                auto currentAngleIndex = videoModel()->index({
                    { topLeft.row(), VideoModel::AllAnglesCol },
                    { i, 0 }
                });

                auto startFrame = videoModel()->index(currentAngleIndex, {
                    { 0, VideoModel::AFrameCol }
                }).data().toInt();

                frame -= startFrame;

                auto centerIndex = videoModel()->index(currentAngleIndex, {
                    { frame, VideoModel::ACenterCol }
                });
                auto edge1Index = videoModel()->index(currentAngleIndex, {
                    { frame, VideoModel::AEdge1Col }
                });
                auto edge2Index = videoModel()->index(currentAngleIndex, {
                    { frame, VideoModel::AEdge2Col }
                });

                QPointF center = centerIndex.data(VideoModel::VideoDataRole).toPointF();
                QPointF edge1 = edge1Index.data(VideoModel::VideoDataRole).toPointF();
                QPointF edge2 = edge2Index.data(VideoModel::VideoDataRole).toPointF();

                AngleItem *ang = v.angles.at(i);
                ang->setPoints(center, edge1, edge2);
            }
        }
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.column() == VideoModel::AllDistancesCol) {
            DistanceItem *line = _videos.at(parent.row()).distances.at(topLeft.row());
            line->setLine(topLeft.data(VideoModel::VideoDataRole).toLineF());
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        if (parent.parent().column() == VideoModel::AllTrajectoriesCol) {
            QPointF pos = topLeft.data(VideoModel::VideoDataRole).toPointF();
            TrajectoryItem *trajectory = _videos.at(parent.parent().row()).trajectories.at(parent.row());
            TrajectoryInstantItem *instant;

            switch (topLeft.column()) {
            case VideoModel::PositionCol:
                instant = trajectory->instantAt(topLeft.row());
                instant? instant->setPos(pos) : void();
                break;
            case VideoModel::LFrameCol:
                if (topLeft.row() == 0) {
                    trajectory->setStartingFrame(topLeft.data().toInt());
                }
                break;
            default:
                break;
            }
        } else if (parent.parent().column() == VideoModel::AllAnglesCol) {
            int frame = model()->index(_currentVideoRow, VideoModel::CurrentFrameCol)
                .data().toInt();

            if (frame == topLeft.row()) {
                AngleItem *angle = _videos[parent.parent().row()].angles[parent.row()];
                switch (topLeft.column()) {
                case VideoModel::ACenterCol:
                    angle->setCenter(topLeft.data(VideoModel::VideoDataRole).toPointF());
                    break;
                case VideoModel::AEdge1Col:
                    angle->setEdge1(topLeft.data(VideoModel::VideoDataRole).toPointF());
                    break;
                case VideoModel::AEdge2Col:
                    angle->setEdge2(topLeft.data(VideoModel::VideoDataRole).toPointF());
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void VideoView::selectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    if (selected.isEmpty()) {
        return;
    }

    QModelIndex selectedIndex = selected.at(0).indexes().at(0);
    _currentVideoRow = selectedIndex.row();

    _view->setScene(_videos.at(_currentVideoRow).scene);
    _view->fitInView(_view->sceneRect(), Qt::KeepAspectRatio);

    _playBar->setPlayData(model()->rowCount(
                              model()->index(
                                  _currentVideoRow,
                                  VideoModel::AllFramesCol)),
                          model()->data(
                              model()->index(_currentVideoRow,
                                             VideoModel::FrameDurationCol)).toInt());
    _status->setJobHandler(static_cast<VideoModel *>(model())->jobHandlerForVideo(_currentVideoRow));
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
            QModelIndex index = model()->index(i, VideoModel::AllDistancesCol);
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

VideoModel* VideoView::videoModel() const
{
    return static_cast<VideoModel *>(model());
}

void VideoView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    if (!parent.isValid()) { // Level 0
        for (int i = start; i <= end; ++i) {
            _videos.insert(i, Video(new QGraphicsScene(_view), 0));
            Video &v = _videos[i];
            v.bgRect = new QGraphicsRectItem();
            v.scene->addItem(v.bgRect);
            v.bgRect->setPen(Qt::NoPen);
        }
    } else if (!parent.parent().isValid()) { // Level 1
        Video &v = _videos[parent.row()];
        for (int i = start; i <= end; ++i) {
            switch (parent.column()) {
            AngleItem *ang;
            TrajectoryItem *traj;
            case VideoModel::AllAnglesCol:
                v.angles << new AngleItem(v.bgRect);
                break;
            case VideoModel::AllDistancesCol:
                v.distances << new DistanceItem(v.bgRect);
                break;
            case VideoModel::AllTrajectoriesCol:
                traj = new TrajectoryItem(v.bgRect);
                traj->setDrawTrajectory(TrajectoryItem::DrawBefore);
                v.trajectories << traj;
                break;
            default:
                break;
            }
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        Video &v = _videos[parent.parent().row()];
        TrajectoryItem *trajectory;

        switch (parent.parent().column()) {
        case VideoModel::AllTrajectoriesCol:
            trajectory = v.trajectories[parent.row()];
            break;
        }

        for (int i = start; i <= end; ++i) {
            switch (parent.parent().column()) {
            case VideoModel::AllTrajectoriesCol:
                trajectory->appendInstant();
                break;
            }
        }
    }
}

void VideoView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    if (!parent.isValid()) { // Level 0
        for (; start <= end; --end) {
            Video v = _videos.takeAt(start);
            delete v.scene;
        }

        _view->setScene(_videos.empty()? _noVideoVideo.scene : _videos.first().scene);
    } else if (!parent.parent().isValid()) { // Level 1
        auto &v = _videos[parent.row()];

        if (parent.column() == VideoModel::AllTrajectoriesCol) {
            for (; start <= end; --end) {
                auto trajectory = v.trajectories.takeAt(start);
                delete trajectory;
            }
        } else if (parent.column() == VideoModel::AllDistancesCol) {
            for (; start <= end; --end) {
                auto distance = v.distances.takeAt(start);
                delete distance;
            }
        } else if (parent.column() == VideoModel::AllAnglesCol) {
            for (; start <= end; --end) {
                auto angle = v.angles.takeAt(start);
                delete angle;
            }
        }
    }
}

void VideoView::beginDistanceCreation()
{
    auto status = new Status::Persistent(_status, tr("Click and drag to measure a distance"));

    connect(_view, SIGNAL(mousePressed(QPointF)), SLOT(distanceFirstPoint(QPointF)));
    connect(_view, SIGNAL(mousePressed(QPointF)), status, SLOT(deleteLater()));
}

static QGraphicsLineItem *guideLine = 0;
static bool calibration = false;

void VideoView::distanceFirstPoint(const QPointF &p)
{
    disconnect(_view, SIGNAL(mousePressed(QPointF)), this, SLOT(distanceFirstPoint(QPointF)));

    Video &currentVideo = _videos[_currentVideoRow];

    guideLine = new QGraphicsLineItem(QLineF(p, p), currentVideo.bgRect);
    guideLine->setPen(QColor(0, 0, 255));

    auto status = new Status::Persistent(_status, tr("Release to finish"));

    connect(_view, SIGNAL(mouseDragged(QPointF)), SLOT(distanceUpdateSecondPoint(QPointF)));
    connect(_view, SIGNAL(mouseReleased(QPointF)), SLOT(distanceEndCreation(QPointF)));
    connect(_view, SIGNAL(mouseReleased(QPointF)), status, SLOT(deleteLater()));
}

void VideoView::distanceUpdateSecondPoint(const QPointF &p)
{
    guideLine->setLine(QLineF(guideLine->line().p1(), p));
}

void VideoView::distanceEndCreation(const QPointF &p)
{
    if (!calibration) {
        static_cast<VideoModel *>(model())->createDistance(guideLine->line(), _currentVideoRow);
    } else {
        endScaleCalibration();
    }

    delete guideLine;
    guideLine = 0;

    new Status::Message(_status, tr("Done"), 3000);

    disconnect(_view, SIGNAL(mouseDragged(QPointF)), this, SLOT(distanceUpdateSecondPoint(QPointF)));
    disconnect(_view, SIGNAL(mouseReleased(QPointF)), this, SLOT(distanceEndCreation(QPointF)));
}

void VideoView::beginAngleCreation()
{
    auto status = new Status::Persistent(_status, tr("Click on the center of the angle"));

    connect(_view, SIGNAL(mousePressed(QPointF)), SLOT(angleCenter(QPointF)));
    connect(_view, SIGNAL(mousePressed(QPointF)), status, SLOT(deleteLater()));
}

static AngleItem *guideAngleItem = 0;

void VideoView::angleCenter(const QPointF& p)
{
    disconnect(_view, SIGNAL(mousePressed(QPointF)), this, SLOT(angleCenter(QPointF)));

    guideAngleItem = new AngleItem(p, p, p, _videos[_currentVideoRow].bgRect);

    auto status = new Status::Persistent(_status, tr("Now click on the first peripheral edge"));

    connect(_view, SIGNAL(mousePressed(QPointF)), SLOT(angleEdge1(QPointF)));
    connect(_view, SIGNAL(mousePressed(QPointF)), status, SLOT(deleteLater()));
}

void VideoView::angleEdge1(const QPointF& p)
{
    disconnect(_view, SIGNAL(mousePressed(QPointF)), this, SLOT(angleEdge1(QPointF)));

    guideAngleItem->setEdge1(p);

    auto status = new Status::Persistent(_status, tr("Now click on the second peripheral edge"));

    connect(_view, SIGNAL(mousePressed(QPointF)), SLOT(angleEdge2(QPointF)));
    connect(_view, SIGNAL(mousePressed(QPointF)),  status, SLOT(deleteLater()));
}

void VideoView::angleEdge2(const QPointF& p)
{
    disconnect(_view, SIGNAL(mousePressed(QPointF)), this, SLOT(angleEdge2(QPointF)));

    guideAngleItem->setEdge2(p);

    int frame = model()->index(_currentVideoRow, VideoModel::CurrentFrameCol).data().toInt();

    auto job = static_cast<VideoModel *>(model())->calculateAngle(
        { guideAngleItem->center(), guideAngleItem->edge1(), guideAngleItem->edge2() },
        frame, _currentVideoRow
    );
    static_cast<VideoModel *>(model())->jobHandlerForVideo(_currentVideoRow)->startJob(job);

    delete guideAngleItem;
    guideAngleItem = 0;
}

void VideoView::calculateTrajectory(const QPointF &p)
{
    disconnect(_view, SIGNAL(mouseReleased(QPointF)), this, SLOT(calculateTrajectory(QPointF)));

    int frame = model()->index(_currentVideoRow, VideoModel::CurrentFrameCol).data().toInt();

    TrajectoryCalcJob *job = static_cast<VideoModel *>(model())->calculateTrajectory
            (p, frame, _currentVideoRow);
    static_cast<VideoModel *>(model())->jobHandlerForVideo(_currentVideoRow)->startJob(job);
}

void VideoView::beginScaleCalibration()
{
    calibration = true;
    beginDistanceCreation();
}

void VideoView::endScaleCalibration()
{
    QDialog d { this };
    Ui_ScaleCalibrationDialog d_ui {};

    d_ui.setupUi(&d);
    if (d.exec() == QDialog::Accepted) {
        auto calibrationIndex = model()->index(_currentVideoRow, VideoModel::CalibrationRatioCol);

        auto ratio = d_ui.lineSizeBox->value() / guideLine->line().length();
        model()->setData(calibrationIndex, ratio);
    }

    calibration = false;
}
