/*
    CVMob - Motion capture program
    Copyright (C) 2013, 2014  The CVMob contributors

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
#include "ui_cvmobmainwindow.h"

#include <model/videomodel.hpp>
#include <model/itemmodels/klinkitemselectionmodel.h>
#include <model/proxies/anglesproxymodel.hpp>
#include <model/proxies/distancesproxymodel.hpp>
#include <model/proxies/plotproxies.hpp>
#include <model/proxies/trajectoriesproxymodel.hpp>
#include <model/proxies/videolistproxymodel.hpp>
#include <QMenu>
#include <QMessageBox>
#include <QPointF>
#include <QStandardItemModel>
#include <QFileDialog>
#include <view/plotitemview.hpp>
#include <view/settings.hpp>
#include <view/videoview.hpp>
#include <view/videolistdelegate.hpp>

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CvMobMainWindow),
    _videoModel(new VideoModel),
    _videoView(new VideoView),
    _settingsWidget(new Settings(this))
{
    _ui->setupUi(this);

    _ui->action_Open->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    _ui->openButton->setDefaultAction(_ui->action_Open);

    VideoListProxyModel *videoNamesModel = new VideoListProxyModel(this);
    videoNamesModel->setSourceModel(_videoModel);
    
    _ui->openedVideosList->setModel(videoNamesModel);
    _ui->openedVideosList->setItemDelegate(new VideoListDelegate(this));
    _videoView->setModel(_videoModel);
    
    KLinkItemSelectionModel *selectionModel =
        new KLinkItemSelectionModel(_videoModel, _ui->openedVideosList->selectionModel());
    _videoView->setSelectionModel(selectionModel);

    DistancesProxyModel *distancesModel = new DistancesProxyModel(this);
    distancesModel->setSourceModel(_videoModel);
    distancesModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    _ui->distancesView->setModel(distancesModel);
    _ui->distancesView->header()->setSectionResizeMode(QHeaderView::Stretch);

    TrajectoriesProxyModel *trajectoriesModel =
            new TrajectoriesProxyModel(this);
    trajectoriesModel->setSourceModel(_videoModel);
    trajectoriesModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    _ui->trajectoriesView->setModel(trajectoriesModel);
    
    AnglesProxyModel *anglesModel = new AnglesProxyModel(this);
    anglesModel->setSourceModel(_videoModel);
    anglesModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    _ui->anglesView->setModel(anglesModel);
    
    QLayout *l = _ui->graphsDockWidgetContents->layout();
    
    PlotItemView *xPlot = new PlotItemView(this);
    auto xModel = new XTrajectoryPlotProxyModel(this);
    xModel->setSourceModel(_videoModel);
    xModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    xPlot->setModel(xModel);
    l->addWidget(xPlot);
    connect(_ui->xGraphCheckBox, &QCheckBox::toggled, xPlot, &QWidget::setVisible);
    
    PlotItemView *yPlot = new PlotItemView(this);
    auto yModel = new YTrajectoryPlotProxyModel(this);
    yModel->setSourceModel(_videoModel);
    yModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    yPlot->setModel(yModel);
    l->addWidget(yPlot);
    connect(_ui->yGraphCheckBox, &QCheckBox::toggled, yPlot, &QWidget::setVisible);
    
    PlotItemView *speedPlot = new PlotItemView(this);
    auto speedModel = new TrajectorySpeedPlotProxyModel(this);
    speedModel->setSourceModel(_videoModel);
    speedModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    speedPlot->setModel(speedModel);
    l->addWidget(speedPlot);
    speedPlot->hide();
    connect(_ui->speedCheckBox, &QCheckBox::toggled, speedPlot, &QWidget::setVisible);
    
    PlotItemView *accelPlot = new PlotItemView(this);
    auto accelModel = new TrajectoryAccelPlotProxyModel(this);
    accelModel->setSourceModel(_videoModel);
    accelModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    accelPlot->setModel(accelModel);
    l->addWidget(accelPlot);
    accelPlot->hide();
    connect(_ui->accelerationCheckBox, &QCheckBox::toggled, accelPlot, &QWidget::setVisible);
    
    PlotItemView *anglePlot = new PlotItemView(this);
    auto angleModel = new AnglePlotProxyModel(this);
    angleModel->setSourceModel(_videoModel);
    angleModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    anglePlot->setModel(angleModel);
    l->addWidget(anglePlot);
    anglePlot->hide();
    connect(_ui->angleCheckBox, &QCheckBox::toggled, anglePlot, &QWidget::setVisible);

    setCentralWidget(_videoView);

    _videoView->showMessage(tr("CVMob developer snapshot"));

    connect(_ui->action_Open, SIGNAL(triggered()), SLOT(openFile()));
    connect(_videoView, SIGNAL(settingsRequested()), _settingsWidget, SLOT(exec()));
    connect(_settingsWidget, SIGNAL(settingsChanged()), _videoModel, SLOT(updateSettings()));
    connect(_settingsWidget, SIGNAL(settingsChanged()), _videoView, SLOT(updateSettings()));
    connect(_settingsWidget, SIGNAL(settingsChanged()), xPlot, SLOT(updateSettings()));
    connect(_settingsWidget, SIGNAL(settingsChanged()), yPlot, SLOT(updateSettings()));
    connect(_settingsWidget, SIGNAL(settingsChanged()), speedPlot, SLOT(updateSettings()));
    connect(_settingsWidget, SIGNAL(settingsChanged()), accelPlot, SLOT(updateSettings()));
    connect(_settingsWidget, SIGNAL(settingsChanged()), anglePlot, SLOT(updateSettings()));
}

void CvMobMainWindow::openFile() {
    QString pathName = QFileDialog::getOpenFileName(this, tr("Open video"), ".",
                                                    tr("All supported video formats (*.avi *.ogv *.wmv *.mp4)"));

    if (pathName.isNull()) {
        return;
    }

    if (_videoModel->openVideo(pathName)) {
        _videoView->showMessage(tr("%1 opened").arg(pathName));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Could not open %1").arg(pathName));
    }
}

CvMobMainWindow::~CvMobMainWindow()
{
	delete _ui;
}
