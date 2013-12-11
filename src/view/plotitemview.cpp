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

#include "plotitemview.hpp"

#include <QBoxLayout>
#include <QSettings>
#include <QTimer>
#include <view/qcustomplot.h>

PlotItemView::PlotItemView(QWidget* parent) :
    QAbstractItemView(parent),
    _plot(new QCustomPlot(this)),
    _title(new QCPPlotTitle(_plot, tr("(untitled)"))),
    _wantsUpdate(false),
    _wasVisibleBefore(false)
{
    _plot->setInteraction(QCP::iRangeDrag, true);
    _plot->setInteraction(QCP::iRangeZoom, true);
    _plot->setInteraction(QCP::iSelectPlottables, true);
    _plot->setInteraction(QCP::iSelectItems, true);
    _plot->setInteraction(QCP::iSelectAxes, true);
    _plot->setInteraction(QCP::iSelectOther, true);
    _plot->plotLayout()->insertRow(0);
    _plot->plotLayout()->addElement(0, 0, _title);
    
    new QBoxLayout(QBoxLayout::TopToBottom, viewport());
    viewport()->layout()->addWidget(_plot);
    
    QSettings set;
    
    _mainTimerId = startTimer(set.value("plot/period", 200).toInt());
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

void PlotItemView::updateSettings()
{
    killTimer(_mainTimerId);
    
    QSettings set;
    
    _mainTimerId = startTimer(set.value("plot/period", 200).toInt());
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
    
    return false;
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

void PlotItemView::reset()
{
    if (!isVisible()) {
        return;
    }
    
    _title->setText(model()->headerData(0, Qt::Horizontal).toString());
    _plot->xAxis->setLabel(model()->headerData(1, Qt::Horizontal).toString());
    _plot->yAxis->setLabel(model()->headerData(2, Qt::Vertical).toString());
    
    _plot->clearGraphs();
    
    for (int i = 0; i < model()->rowCount(); ++i) {
        _plot->addGraph();
        
        QModelIndex iIndex = model()->index(i, 0);
        for (int j = 0; j < model()->rowCount(iIndex); ++j) {
            _plot->graph(i)->addData(model()->index(j, 0, iIndex).data().toDouble(),
                                     model()->index(j, 1, iIndex).data().toDouble());
        }
    }
    
    _plot->rescaleAxes();
    _plot->replot();
}

void PlotItemView::timerEvent(QTimerEvent* e)
{
    if (e->timerId() == _mainTimerId && _wantsUpdate) {
        _plot->rescaleAxes();
        _plot->replot();
        _wantsUpdate = false;
    }
}

void PlotItemView::hideEvent(QHideEvent* e)
{
    _wasVisibleBefore = false;
    
    QWidget::hideEvent(e);
}

void PlotItemView::showEvent(QShowEvent* e)
{
    if (!_wasVisibleBefore) {
        reset();
        _wasVisibleBefore = true;
    }
    
    QWidget::showEvent(e);
}

void PlotItemView::dataChanged(const QModelIndex& topLeft, const QModelIndex& , const QVector< int >& roles)
{
    if (!isVisible()) {
        return;
    }
    
    if (!topLeft.parent().isValid() || topLeft.column() != 1) {
        return;
    }
    
    int graphRow = topLeft.parent().row();
    QModelIndex graphIndex = model()->index(topLeft.parent().row(), 0);
    double key = model()->index(topLeft.row(), 0, graphIndex).data().toDouble();
    QCPGraph *graph = _plot->graph(graphRow);
    
    graph->removeData(key);
    graph->addData(model()->index(topLeft.row(), 0, graphIndex).data().toDouble(),
                   model()->index(topLeft.row(), 1, graphIndex).data().toDouble());
    
    _wantsUpdate = true;
}

void PlotItemView::rowsInserted(const QModelIndex& parent, int start, int end)
{
    if (!isVisible()) {
        return;
    }
    
    for (int i = start; i <= end; ++i) {
        if (parent.isValid()) {
            continue;
        }
        
        _plot->addGraph();
    }
}
