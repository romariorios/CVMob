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

#ifndef PLOTITEMVIEW_H
#define PLOTITEMVIEW_H

#include <QAbstractItemView>

class QCPPlotTitle;
class QCustomPlot;

class PlotItemView : public QAbstractItemView
{
    Q_OBJECT
    
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
    void timerEvent(QTimerEvent *e);
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);
    
public slots:
    void reset();
    
protected slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &, // Topleft always equals
                                                                      // bottomright
                     const QVector<int> &roles = QVector<int>());
    void rowsInserted(const QModelIndex &parent, int start, int end);
    
private:
    QCustomPlot *_plot;
    QCPPlotTitle *_title;
    bool _wantsUpdate;
    int _mainTimerId;
    bool _wasVisibleBefore;

};

#endif // PLOTITEMVIEW_H
