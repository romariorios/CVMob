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

#ifndef PLOTITEMVIEW_H
#define PLOTITEMVIEW_H

#include <QAbstractItemView>

class QCustomPlot;

class PlotItemView : public QAbstractItemView
{
public:
    explicit PlotItemView(QWidget* parent = 0);
    QModelIndex indexAt(const QPoint& point) const;
    void scrollTo(const QModelIndex& index, QAbstractItemView::ScrollHint hint);
    QRect visualRect(const QModelIndex& index) const;

protected:
    QRegion visualRegionForSelection(const QItemSelection& selection) const;
    void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command);
    bool isIndexHidden(const QModelIndex& index) const;
    int verticalOffset() const;
    int horizontalOffset() const;
    QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    
private:
    QCustomPlot *_plot;

};

#endif // PLOTITEMVIEW_H
