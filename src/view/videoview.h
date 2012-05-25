#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#include <QtGui/QAbstractItemView>

class QGraphicsView;
class QGraphicsRectItem;

// FIXME: Qt doesn't support multiple inheritance
class VideoView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit VideoView(QWidget *parent = 0);

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
    // This method returns the biggest rect with the given width:height proportion that fits the
    // given rect
    const QRectF fitRectWithProportion(const QRectF &rect, qreal proportion) const;

    QGraphicsView *_view;
    QGraphicsRectItem *_bgRect;

protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

signals:

public slots:

};

#endif // VIDEOVIEW_H
