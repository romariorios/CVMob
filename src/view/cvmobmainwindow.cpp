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

#include "cvmobmainwindow.h"
#include "ui_cvmobmainwindow.h"

#include <model/videomodel.hpp>
#include <model/distancesproxymodel.hpp>
#include <model/lineartrajectoriesproxymodel.hpp>
#include <QtCore/QPointF>
#include <QtGui/QStandardItemModel>
#include <QtGui/QFileDialog>
#include <view/videoview.h>

#include <QDebug>

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CvMobMainWindow),
    _videoModel(new VideoModel),
    _videoView(new VideoView)
{
    _ui->setupUi(this);

    _ui->openButton->setDefaultAction(_ui->action_Open);

    _ui->openedVideosList->setModel(_videoModel);
    _videoView->setModel(_videoModel);
    _videoView->setSelectionModel(_ui->openedVideosList->selectionModel());

    DistancesProxyModel *distancesModel = new DistancesProxyModel(this);
    distancesModel->setSourceModel(_videoModel);
    distancesModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    _ui->distancesView->setModel(distancesModel);
    _ui->distancesView->header()->setResizeMode(QHeaderView::Stretch);

    LinearTrajectoriesProxyModel *linearTrajectoriesModel =
            new LinearTrajectoriesProxyModel(this);
    linearTrajectoriesModel->setSourceModel(_videoModel);
    linearTrajectoriesModel->setSelectionModel(_ui->openedVideosList->selectionModel());
    _ui->trajectoriesView->setModel(linearTrajectoriesModel);

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
