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

#include "cvmobmainwindow.hpp"
#include "ui_cvmobmainwindow.h"

#include <model/videomodel.hpp>
#include <model/itemmodels/klinkitemselectionmodel.h>
#include <model/proxies/anglesproxymodel.hpp>
#include <model/proxies/distancesproxymodel.hpp>
#include <model/proxies/trajectoriesproxymodel.hpp>
#include <model/proxies/videolistproxymodel.hpp>
#include <QPointF>
#include <QStandardItemModel>
#include <QFileDialog>
#include <view/videoview.hpp>

#include <QDebug>

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CvMobMainWindow),
    _videoModel(new VideoModel),
    _videoView(new VideoView)
{
    _ui->setupUi(this);

    _ui->openButton->setDefaultAction(_ui->action_Open);

    VideoListProxyModel *videoNamesModel = new VideoListProxyModel(this);
    videoNamesModel->setSourceModel(_videoModel);
    
    _ui->openedVideosList->setModel(videoNamesModel);
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

    setCentralWidget(_videoView);

    _videoView->showMessage(tr("CVMob developer snapshot"));

    connect(_ui->action_Open, SIGNAL(triggered()), SLOT(openFile()));
}

void CvMobMainWindow::openFile() {
    QString pathName = QFileDialog::getOpenFileName(this, tr("Open video"), ".",
                                                    tr("All supported video formats (*.avi *.ogv *.wmv)"));

    if (pathName.isNull()) {
        return;
    }

    if (_videoModel->openVideo(pathName)) {
        _videoView->showMessage(tr("%1 opened").arg(pathName));
    } else {
        _videoView->showMessage(tr("Could not open %1").arg(pathName));
    }
}

CvMobMainWindow::~CvMobMainWindow()
{
	delete _ui;
}
