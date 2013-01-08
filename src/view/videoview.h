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

#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#include <QtGui/QAbstractItemView>

#include <QtCore/QList>

class PlayBar;
class QGraphicsLineItem;
class QGraphicsRectItem;
class QGraphicsScene;
class VideoGraphicsView;
class VideoStatus;

class VideoView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit VideoView(QWidget *parent = 0);
    ~VideoView();

    void scrollTo(const QModelIndex &index, ScrollHint hint);
    QModelIndex indexAt(const QPoint &point) const;
    QRect visualRect(const QModelIndex &index) const;

    void showMessage(const QString &message, int duration = 5000);

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    int horizontalOffset() const;
    int verticalOffset() const;
    bool isIndexHidden(const QModelIndex &index) const;
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;
    void resizeEvent(QResizeEvent *event);

private:
    struct Video
    {
        QGraphicsScene *scene;
        QGraphicsRectItem *bgRect;
        QList<QGraphicsLineItem *> distances;

        Video(QGraphicsScene *e_s, QGraphicsRectItem *e_bgr) :
            scene(e_s),
            bgRect(e_bgr)
        {}
    };

    VideoGraphicsView *_view;
    QList<Video> _videos;
    Video _noVideoVideo;
    int _currentVideoRow;
    PlayBar *_playBar;
    VideoStatus *_status;

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &); // topLeft is always equal
                                                                       // to bottomRight.
    void selectionChanged(const QItemSelection &selected, const QItemSelection &);
    void rowsInserted(const QModelIndex &parent, int start, int end);

private slots:
    void changeFrame(int frame);
    void beginDistanceCreation();
    void beginLTrajectoryCalculation();

    void distanceFirstPoint(const QPointF &p);
    void distanceUpdateSecondPoint(const QPointF &p);
    void distanceEndCreation(const QPointF &p);

    void calculateLTrajectoryFromPoint(const QPointF &p);
};

#endif // VIDEOVIEW_H
