#ifndef VIDEOGRAPHICSVIEW_HPP
#define VIDEOGRAPHICSVIEW_HPP

#include <QGraphicsView>

#include <QPointF>
#include <QSizeF>

class VideoGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit VideoGraphicsView(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    bool _pressed;
    
signals:
    void mousePressed(const QPointF &p);
    void mouseReleased(const QPointF &p);
    void mouseDragged(const QPointF &p);
};

#endif // VIDEOGRAPHICSVIEW_HPP
