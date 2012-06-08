#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#include <QtGui/QAbstractItemView>

#include <QtCore/QList>

class PlayBar;
class QGraphicsView;
class QGraphicsRectItem;
class QGraphicsScene;

// FIXME: Qt doesn't support multiple inheritance
class VideoView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit VideoView(QWidget *parent = 0);
    ~VideoView();

    void scrollTo(const QModelIndex &index, ScrollHint hint);
    QModelIndex indexAt(const QPoint &point) const;
    QRect visualRect(const QModelIndex &index) const;

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    int horizontalOffset() const;
    int verticalOffset() const;
    bool isIndexHidden(const QModelIndex &index) const;
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    QRegion visualRegionForSelection(const QItemSelection &selection) const;
    void resizeEvent(QResizeEvent *event);

private:
    QGraphicsView *_view;
    QList<QGraphicsScene *> _scenes;
    QGraphicsScene *_noVideoScene;
    PlayBar *_playBar;

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &); // topLeft is always equal
                                                                       // to bottomRight.
    void selectionChanged(const QItemSelection &selected, const QItemSelection &);
    void rowsInserted(const QModelIndex &parent, int start, int end);
};

#endif // VIDEOVIEW_H
