#include "videographicsview.hpp"

#include <QMouseEvent>

#include <QDebug>

VideoGraphicsView::VideoGraphicsView(QWidget *parent) :
    QGraphicsView(parent),
    _pressed(false)
{}

void VideoGraphicsView::mousePressEvent(QMouseEvent *event)
{
    _pressed = true;
    emit mousePressed(mapToPositionInScene(event->posF()));
}

void VideoGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    _pressed = false;
    emit mouseReleased(mapToPositionInScene(event->posF()));
}


void VideoGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (_pressed) {
        emit mouseDragged(mapToPositionInScene(event->posF()));
    }
}

const QPointF VideoGraphicsView::mapToPositionInScene(QPointF p) const
{
    p.setX(sceneRect().width() * (p.x() / width()));
    p.setY(sceneRect().height() * (p.y() / height()));
    return p;
}
