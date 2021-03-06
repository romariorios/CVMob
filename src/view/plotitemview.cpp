/*
    CVMob - Motion capture program
    Copyright (C) 2013--2015  The CVMob contributors

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
#include <QDialog>
#include <QLineEdit>
#include <QtGui/QMouseEvent>
#include <QtGui/QPen>
#include <QSettings>
#include <QTimer>

#include <model/videomodel.hpp>

enum {
    __HACKCurrentFrameCol = 2
};

PlotItemView::PlotItemView(QWidget* parent) :
    QAbstractItemView{parent}
{
    _plot.setInteraction(QCP::iRangeDrag, true);
    _plot.setInteraction(QCP::iRangeZoom, true);
    _plot.setInteraction(QCP::iSelectPlottables, true);
    _plot.setInteraction(QCP::iSelectItems, true);
    _plot.setInteraction(QCP::iSelectAxes, true);
    _plot.setInteraction(QCP::iSelectOther, true);
    _plot.plotLayout()->insertRow(0);
    _plot.plotLayout()->addElement(0, 0, &_title);

    _timeLine.start->setCoords(0, -10);
    _timeLine.end->setCoords(0, 10);
    _plot.addItem(&_timeLine);

    connect(&_plot, &QCustomPlot::mousePress, this, &PlotItemView::onPlotMousePress);

    new QBoxLayout(QBoxLayout::TopToBottom, viewport());
    viewport()->layout()->addWidget(&_plot);
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
    if (_timerRunning) {
        killTimer(_mainTimerId);
    }
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

    _title.setText(model()->headerData(0, Qt::Horizontal).toString());
    _plot.xAxis->setLabel(model()->headerData(1, Qt::Horizontal).toString());
    _plot.yAxis->setLabel(model()->headerData(2, Qt::Vertical).toString());
    _plot.clearGraphs();

    for (int i = 0; i < model()->rowCount(); ++i) {
        auto g = _plot.addGraph();
        g->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
        g->setPen(model()->index(i, VideoModel::TrajectoryColorCol).data().value<QColor>());

        QModelIndex iIndex = model()->index(i, 0);
        for (int j = 0; j < model()->rowCount(iIndex); ++j) {
            g->addData(
                model()->index(j, 0, iIndex).data().toDouble(),
                model()->index(j, 1, iIndex).data().toDouble());
        }
    }

    _plot.rescaleAxes();
    _plot.replot();
}

void PlotItemView::timerEvent(QTimerEvent* e)
{
    if (e->timerId() != _mainTimerId) {
        return QAbstractItemView::timerEvent(e);
    }

    if (_wantsUpdate) {
        updatePlot();
    } else {
        killTimer(_mainTimerId);
        _timerRunning = false;
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

    if (!topLeft.parent().isValid()) {
        if (topLeft.column() == __HACKCurrentFrameCol) {
            auto x = topLeft.data().toInt();

            _timeLine.start->setCoords(x, -10000);
            _timeLine.end->setCoords(x, 10000);
        }

        if (topLeft.column() == VideoModel::TrajectoryColorCol) {
            _plot.graph(topLeft.row())->setPen(topLeft.data().value<QColor>());
        }

        return requestUpdate();
    }

    if (topLeft.column() != 1) {
        return;
    }

    int graphRow = topLeft.parent().row();
    QModelIndex graphIndex = model()->index(topLeft.parent().row(), 0);
    double key = model()->index(topLeft.row(), 0, graphIndex).data().toDouble();
    QCPGraph *graph = _plot.graph(graphRow);

    graph->removeData(key);
    graph->addData(model()->index(topLeft.row(), 0, graphIndex).data().toDouble(),
                   model()->index(topLeft.row(), 1, graphIndex).data().toDouble());

    requestUpdate();
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

        _plot.addGraph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
    }
}

void PlotItemView::rowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    if (!parent.isValid()) {
        for (; start <= end; --end) {
            _plot.removeGraph(start);
        }
    }

    requestUpdate();
}

void PlotItemView::onPlotMousePress(QMouseEvent* e)
{
    int i = _plot.graphCount();

    if (e->button() != Qt::RightButton || i <= 0) {
        return;
    }

    double ax = e->pos().x();
    double ay = e->pos().y();
    ax = _plot.xAxis->pixelToCoord(ax);
    ay = _plot.yAxis->pixelToCoord(ay);
    double dMin = 999999999;
    double xMin, yMin;

    for (int j = 0; j < i; j++) {
        QCPData b = _plot.graph(j)->data()->lowerBound(ax).value();
        double dx = ax-b.key;
        double dy = ay-b.value;
        double d = sqrt(dx * dx + dy * dy); // calcs distance

        if (d < dMin) {
            dMin = d;
            xMin = b.key;
            yMin = b.value;
        }
    }

    _plot.addGraph();
    _plot.graph(i)->setPen(QPen { QColor { 255, 0, 0 } });
    _plot.graph(i)->setScatterStyle(QCPScatterStyle { QCPScatterStyle::ssCircle, 10 });
    _plot.graph(i)->addData(xMin, yMin);

    QDialog coordinatesDialog { this };
    coordinatesDialog.setWindowTitle(tr("Point"));
    QVBoxLayout l { &coordinatesDialog };
    QLineEdit coordinates { tr("(%1, %2)").arg(xMin).arg(yMin), &coordinatesDialog };
    coordinates.setReadOnly(true);
    l.addWidget(&coordinates);
    coordinatesDialog.layout()->setSizeConstraint(QLayout::SetFixedSize);

    requestUpdate();
    coordinatesDialog.exec();

    _plot.graph(i)->clearData();
    _plot.removeGraph(i);
    requestUpdate();
}

void PlotItemView::updatePlot()
{
    _plot.rescaleAxes();
    _plot.replot();
    _wantsUpdate = false;
}

int PlotItemView::startMainTimer()
{
    return startTimer(QSettings {}.value("plot/period", 200).toInt());
}

void PlotItemView::requestUpdate()
{
    if (_timerRunning) {
        _wantsUpdate = true;
    } else {
        updatePlot();
        _mainTimerId = startMainTimer();
        _timerRunning = true;
    }
}
