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
    emit mousePressed(mapToScene(event->pos()));
}

void VideoGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    _pressed = false;
    emit mouseReleased(mapToScene(event->pos()));
}


void VideoGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (_pressed) {
        emit mouseDragged(mapToScene(event->pos()));
    }
}

void VideoGraphicsView::resizeEvent(QResizeEvent* event)
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    
    QGraphicsView::resizeEvent(event);
}
