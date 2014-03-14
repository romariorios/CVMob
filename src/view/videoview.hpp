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

#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#include <QAbstractItemView>

#include <QList>

class AngleItem;
class DistanceItem;
class PlayBar;
class QGraphicsLineItem;
class QGraphicsRectItem;
class QGraphicsScene;
class TrajectoryItem;
class VideoGraphicsView;
class VideoStatus;
class VideoModel;

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
    
signals:
    void settingsRequested();

private:
    struct Video
    {
        QGraphicsScene *scene;
        QGraphicsRectItem *bgRect;
        QList<AngleItem *> angles;
        QList<DistanceItem *> distances;
        QList<TrajectoryItem *> trajectories;

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
    
    VideoModel *videoModel() const;
    
public slots:
    void updateSettings();

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &, // topLeft == bottomRight
                     const QVector<int> & = QVector<int>());          // roles is always empty
    void selectionChanged(const QItemSelection &selected, const QItemSelection &);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

private slots:
    void beginDistanceCreation();

    void distanceFirstPoint(const QPointF &p);
    void distanceUpdateSecondPoint(const QPointF &p);
    void distanceEndCreation(const QPointF &p);
    
    void beginAngleCreation();
    
    void angleCenter(const QPointF &p);
    void angleEdge1(const QPointF &p);
    void angleEdge2(const QPointF &p);

    void calculateTrajectory(const QPointF &p);
};

#endif // VIDEOVIEW_H
