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
#include <tuple>
#include <view/videolistdelegate.hpp>

#include <cvmob_version.hpp>

using namespace std;

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

    for (auto tuple : {
        make_tuple(&_xPlot, &_xPlotModel, _ui.xGraphCheckBox),
        make_tuple(&_yPlot, &_yPlotModel, _ui.yGraphCheckBox),
        make_tuple(&_speedPlot, &_speedPlotModel, _ui.speedCheckBox),
        make_tuple(&_accelPlot, &_accelPlotModel, _ui.accelerationCheckBox),
        make_tuple(&_anglePlot, &_anglePlotModel, _ui.angleCheckBox)
    }) {
        auto w = get<0>(tuple);
        auto p = get<1>(tuple);
        auto checkBox = get<2>(tuple);

        p->setSourceModel(&_videoModel);
        p->setSelectionModel(_ui.openedVideosList->selectionModel());
        w->setModel(p);
        l->addWidget(w);
        w->hide();
        connect(checkBox, &QCheckBox::toggled, w, &QWidget::setVisible);
    }

    _xPlot.show();
    _yPlot.show();

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
