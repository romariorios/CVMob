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

#include "cvmobmainwindow.hpp"

#include <KItemModels/KLinkItemSelectionModel>
#include <QMenu>
#include <QMessageBox>
#include <QPointF>
#include <QStandardItemModel>
#include <QFileDialog>
#include <view/videolistdelegate.hpp>

#include <cvmob_version.hpp>

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow{parent},
    _xPlotModel{
        VideoModel::AllTrajectoriesCol,
        tr("X (Trajectory)"),
        VideoModel::PositionCol,
        VideoModel::LFrameCol,
        tr("X"),
        [](const QModelIndex &i)
        {
            return i.data().toPointF().x();
        }
    },
    _yPlotModel{
        VideoModel::AllTrajectoriesCol,
        tr("Y (trajectory)"),
        VideoModel::PositionCol,
        VideoModel::LFrameCol,
        tr("Y"),
        [](const QModelIndex &i)
        {
            return i.data().toPointF().y();
        }
    },
    _speedPlotModel{
        VideoModel::AllTrajectoriesCol,
        tr("Speed"),
        VideoModel::LSpeedCol,
        VideoModel::LFrameCol,
        tr("Speed (pxl/frame)"),
        [](const QModelIndex &i)
        {
            return QLineF{QPointF{0, 0}, i.data().toPointF()}.length();
        }
    },
    _accelPlotModel{
        VideoModel::AllTrajectoriesCol,
        tr("Acceleration"),
        VideoModel::LAccelCol,
        VideoModel::LFrameCol,
        tr("Acceleration (pxl/frame^2)"),
        [](const QModelIndex &i)
        {
            return QLineF{QPointF{0, 0}, i.data().toPointF()}.length();
        }
    },
    _anglePlotModel{
        VideoModel::AllAnglesCol,
        tr("Angle"),
        VideoModel::AngleCol,
        VideoModel::AFrameCol,
        tr("Angle (radians)")
    }
{
    _ui.setupUi(this);
    _ui.toolBar->hide();

    _ui.action_Open->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    _ui.openButton->setDefaultAction(_ui.action_Open);

    _videoNamesModel.setSourceModel(&_videoModel);

    _ui.openedVideosList->setModel(&_videoNamesModel);
    _ui.openedVideosList->setItemDelegate(new VideoListDelegate(this));
    _videoView.setModel(&_videoModel);
    _videoView.setSelectionModel(new KLinkItemSelectionModel{
        &_videoModel,
        _ui.openedVideosList->selectionModel(),
        this});

    _distancesModel.setSourceModel(&_videoModel);
    _distancesModel.setSelectionModel(_ui.openedVideosList->selectionModel());
    _ui.distancesView->setModel(&_distancesModel);
    _ui.distancesView->header()->setSectionResizeMode(QHeaderView::Stretch);

    _trajectoriesModel.setSourceModel(&_videoModel);
    _trajectoriesModel.setSelectionModel(_ui.openedVideosList->selectionModel());
    _ui.trajectoriesView->setModel(&_trajectoriesModel);

    _anglesModel.setSourceModel(&_videoModel);
    _anglesModel.setSelectionModel(_ui.openedVideosList->selectionModel());
    _ui.anglesView->setModel(&_anglesModel);

    QLayout *l = _ui.graphsDockWidgetContents->layout();

    _xPlotModel.setSourceModel(&_videoModel);
    _xPlotModel.setSelectionModel(_ui.openedVideosList->selectionModel());
    _xPlot.setModel(&_xPlotModel);
    l->addWidget(&_xPlot);
    connect(_ui.xGraphCheckBox, &QCheckBox::toggled, &_xPlot, &QWidget::setVisible);

    _yPlotModel.setSourceModel(&_videoModel);
    _yPlotModel.setSelectionModel(_ui.openedVideosList->selectionModel());
    _yPlot.setModel(&_yPlotModel);
    l->addWidget(&_yPlot);
    connect(_ui.yGraphCheckBox, &QCheckBox::toggled, &_yPlot, &QWidget::setVisible);

    _speedPlotModel.setSourceModel(&_videoModel);
    _speedPlotModel.setSelectionModel(_ui.openedVideosList->selectionModel());
    _speedPlot.setModel(&_speedPlotModel);
    l->addWidget(&_speedPlot);
    _speedPlot.hide();
    connect(_ui.speedCheckBox, &QCheckBox::toggled, &_speedPlot, &QWidget::setVisible);

    _accelPlotModel.setSourceModel(&_videoModel);
    _accelPlotModel.setSelectionModel(_ui.openedVideosList->selectionModel());
    _accelPlot.setModel(&_accelPlotModel);
    l->addWidget(&_accelPlot);
    _accelPlot.hide();
    connect(_ui.accelerationCheckBox, &QCheckBox::toggled, &_accelPlot, &QWidget::setVisible);

    _anglePlotModel.setSourceModel(&_videoModel);
    _anglePlotModel.setSelectionModel(_ui.openedVideosList->selectionModel());
    _anglePlot.setModel(&_anglePlotModel);
    l->addWidget(&_anglePlot);
    _anglePlot.hide();
    connect(_ui.angleCheckBox, &QCheckBox::toggled, &_anglePlot, &QWidget::setVisible);

    setCentralWidget(&_videoView);

    _videoView.showMessage(tr("CVMob %1").arg(CVMobVersionString));

    connect(_ui.action_Open, SIGNAL(triggered()), SLOT(openFile()));

    connect(_ui.actionShow_opened_videos, SIGNAL(triggered(bool)),
            _ui.openedVideosDockWidget, SLOT(setVisible(bool)));
    connect(_ui.actionShow_opened_videos, SIGNAL(triggered(bool)),
            SLOT(setDockWasShown(bool)));

    connect(_ui.actionShow_data_tables, SIGNAL(triggered(bool)),
            _ui.dataTablesDockWidget, SLOT(setVisible(bool)));
    connect(_ui.actionShow_data_tables, SIGNAL(triggered(bool)),
            SLOT(setDockWasShown(bool)));

    connect(_ui.actionShow_graphs, SIGNAL(triggered(bool)),
            _ui.graphsDockWidget, SLOT(setVisible(bool)));
    connect(_ui.actionShow_graphs, SIGNAL(triggered(bool)),
            SLOT(setDockWasShown(bool)));

    connect(_ui.openedVideosDockWidget, &CVMDockWidget::closed, _ui.actionShow_opened_videos, [=]()
    {
        _ui.actionShow_opened_videos->setChecked(false);
        setDockWasShown(false);
    });

    connect(_ui.dataTablesDockWidget, &CVMDockWidget::closed, _ui.actionShow_opened_videos, [=]()
    {
        _ui.actionShow_data_tables->setChecked(false);
        setDockWasShown(false);
    });

    connect(_ui.graphsDockWidget, &CVMDockWidget::closed, _ui.actionShow_opened_videos, [=]()
    {
        _ui.actionShow_graphs->setChecked(false);
        setDockWasShown(false);
    });

    connect(&_videoView, SIGNAL(settingsRequested()), &_settingsWidget, SLOT(exec()));
    connect(&_settingsWidget, SIGNAL(settingsChanged()), &_videoModel, SLOT(updateSettings()));
    connect(&_settingsWidget, SIGNAL(settingsChanged()), &_videoView, SLOT(updateSettings()));
    connect(&_settingsWidget, SIGNAL(settingsChanged()), &_xPlot, SLOT(updateSettings()));
    connect(&_settingsWidget, SIGNAL(settingsChanged()), &_yPlot, SLOT(updateSettings()));
    connect(&_settingsWidget, SIGNAL(settingsChanged()), &_speedPlot, SLOT(updateSettings()));
    connect(&_settingsWidget, SIGNAL(settingsChanged()), &_accelPlot, SLOT(updateSettings()));
    connect(&_settingsWidget, SIGNAL(settingsChanged()), &_anglePlot, SLOT(updateSettings()));
}

void CvMobMainWindow::openFile() {
    QString pathName = QFileDialog::getOpenFileName(this, tr("Open video"), ".",
                                                    tr("All supported video formats (*.avi *.ogv *.wmv *.mp4)"));

    if (pathName.isNull()) {
        return;
    }

    auto videoRow = _videoModel.openVideo(pathName);
    if (videoRow >= 0) {
        _videoView.showMessage(tr("%1 opened").arg(pathName));
        _ui.openedVideosList->selectionModel()->select(
            _ui.openedVideosList->model()->index(videoRow, 0),
            QItemSelectionModel::ClearAndSelect
        );
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Could not open %1").arg(pathName));
    }
}

void CvMobMainWindow::setDockWasShown(bool shown)
{
    if (shown) {
        if (--_closedDocks == 0) {
            _ui.toolBar->hide();
        }
    } else if (_closedDocks++ == 0) {
        _ui.toolBar->show();
    }
}
