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

#include "videolistdelegate.hpp"

#include <QApplication>
#include <QMouseEvent>

VideoListDelegate::VideoListDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{}

void VideoListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid()) {
        return;
    }
    
    QStyledItemDelegate::paint(painter, option, index);
    
    if (option.state & QStyle::State_Selected) {
        QPixmap closePixmap = QApplication::style()->standardPixmap(QStyle::SP_DialogCloseButton);
        QRect closeRect = closePixmap.rect();
        closeRect.setX(option.rect.width() - closePixmap.width());
        closeRect.setWidth(closePixmap.width());
        QApplication::style()->drawItemPixmap(painter, closeRect, 0, closePixmap);
    }
}

bool VideoListDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() != QEvent::MouseButtonRelease) {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
    
    QPixmap closePixmap = QApplication::style()->standardPixmap(QStyle::SP_DialogCloseButton);
    QRect closeRect = closePixmap.rect();
    closeRect.setX(option.rect.width() - closePixmap.width());
    closeRect.setWidth(closePixmap.width());
    QPoint clickedPoint = static_cast<QMouseEvent *>(event)->pos();
    
    if (!closeRect.contains(clickedPoint)) {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
    
    return model->removeRow(index.row());
}

