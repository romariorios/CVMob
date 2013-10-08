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

#include "plotitemview.hpp"

#include <QBoxLayout>
#include <view/qcustomplot.h>

PlotItemView::PlotItemView(QWidget* parent) :
    QAbstractItemView(parent),
    _plot(new QCustomPlot(this))
{
    new QBoxLayout(QBoxLayout::TopToBottom, viewport());
    viewport()->layout()->addWidget(_plot);
}

QModelIndex PlotItemView::indexAt(const QPoint& point) const
{
    // TODO
    Q_UNUSED(point)
    
    return QModelIndex();
}

void PlotItemView::scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint)
{
    // TODO
    Q_UNUSED(index)
    Q_UNUSED(hint)
}

QRect PlotItemView::visualRect(const QModelIndex& index) const
{
    // TODO
    Q_UNUSED(index)
    
    return QRect();
}

QRegion PlotItemView::visualRegionForSelection(const QItemSelection& selection) const
{
    // TODO
    Q_UNUSED(selection)
    
    return QRegion();
}

void PlotItemView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
{
    // TODO
    Q_UNUSED(rect)
    Q_UNUSED(command)
}

bool PlotItemView::isIndexHidden(const QModelIndex& index) const
{
    // TODO
    Q_UNUSED(index)
    
    return true;
}

int PlotItemView::verticalOffset() const
{
    // TODO
    return 0;
}

int PlotItemView::horizontalOffset() const
{
    // TODO
    return 0;
}

QModelIndex PlotItemView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    // TODO
    Q_UNUSED(cursorAction)
    Q_UNUSED(modifiers)
    
    return QModelIndex();
}
