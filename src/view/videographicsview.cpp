/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

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

#include "videographicsview.hpp"

#include <QMouseEvent>

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
