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
    // "a" and "b" are dimensions
    // sceneRect.lengthOfA() / sceneRect.lengthOfB() < lengthOfA() / lengthOfB()
    // The "dominant" dimension is the dimension which defines the size of the scene

    const bool isWidthDominant = width() / height() > sceneRect().width() / sceneRect().height();
    const double sa = isWidthDominant? sceneRect().height() : sceneRect().width();
    const double sb = isWidthDominant? sceneRect().width() : sceneRect().height();
    const double va = isWidthDominant? height() : width();
    const double vb = isWidthDominant? width() : height();
    const double a = isWidthDominant? p.y() : p.x();
    const double b = isWidthDominant? p.x() : p.y();

    const double as = (a - va / 2) * (sb / vb) + sa / 2;
    const double bs = b * (sb / vb);

    p.setX(isWidthDominant? bs : as);
    p.setY(isWidthDominant? as : bs);
    return p;
}
