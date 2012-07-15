/*
    CVMob - Motion capture program
    Copyright (C) 2012  The CVMob contributors

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

    setCentralWidget(_videoView);

    connect(_ui->action_Open, SIGNAL(triggered()), SLOT(openFile()));

    _videoModel->insertRow(0);
    _videoModel->insertColumns(0, VideoModel::VideoColumnCount);
    _videoModel->setData(_videoModel->index(0, VideoModel::FileNameColumn),
                         "test.avi", VideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, VideoModel::CurrentFrameColumn),
                         0, VideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, VideoModel::FrameDurationColumn),
                         100, VideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, VideoModel::FrameSizeColumn),
                         QSizeF(640, 480), VideoModel::VideoSceneEditRole);

    QModelIndex distancesIndex = _videoModel->index(0, VideoModel::DistancesColumn);
    QModelIndex lineIndex = _videoModel->index(0, 0, distancesIndex);

    _videoModel->insertRow(0, distancesIndex);
    _videoModel->setData(lineIndex, QLineF(0, 0, 100, 100),
                         VideoModel::VideoSceneEditRole);

    _videoModel->insertRow(1);
    _videoModel->setData(_videoModel->index(1, VideoModel::FileNameColumn),
                         "test2.avi", VideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(1, VideoModel::CurrentFrameColumn),
                         0, VideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(1, VideoModel::FrameDurationColumn),
                         100, VideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(1, VideoModel::FrameSizeColumn),
                         QSizeF(400, 300), VideoModel::VideoSceneEditRole);

    distancesIndex = _videoModel->index(1, VideoModel::DistancesColumn);
    lineIndex = _videoModel->index(0, 0, distancesIndex);

    _videoModel->insertRow(0, distancesIndex);
    _videoModel->setData(lineIndex, QLineF(60, 400, 200, 80),
                         VideoModel::VideoSceneEditRole);
}

void CvMobMainWindow::openFile() {
    QString pathName = QFileDialog::getOpenFileName(this, tr("Open video"), ".", tr("Videoclips (*.avi)"));

    if (pathName.isNull()) {
        return;
    }

    int row = _videoModel->rowCount();
    QStringList path = pathName.split('/');
    QString fileName = path.at(path.size() - 1);
    QModelIndex fileIndex = _videoModel->index(row, 0);

    _videoModel->insertRow(row);
    _videoModel->setData(fileIndex, fileName, VideoModel::VideoSceneEditRole);
    _ui->openedVideosList->setCurrentIndex(fileIndex);
}

CvMobMainWindow::~CvMobMainWindow()
{
	delete _ui;
}
