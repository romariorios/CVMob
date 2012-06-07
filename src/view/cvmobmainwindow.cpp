#include "cvmobmainwindow.h"
#include "ui_cvmobmainwindow.h"

#include <model/cvmobvideomodel.hpp>
#include <model/distancesproxymodel.hpp>
#include <QtCore/QPointF>
#include <QtGui/QStandardItemModel>
#include <QtGui/QFileDialog>
#include <view/videoview.h>

#include "graphs/Plot.h"

#include <QDebug>

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CvMobMainWindow),
    _videoModel(new CvmobVideoModel),
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
    _videoModel->insertColumns(0, 4);
    _videoModel->setData(_videoModel->index(0, 0), "test.avi", CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, 1), 0, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, 2), 600, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, 3), 100, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->insertRow(0, _videoModel->index(0, 0));
    _videoModel->setData(_videoModel->index(0, 0, _videoModel->index(0, 0)), QLineF(0, 0, 100, 100),
                         CvmobVideoModel::VideoSceneEditRole);
    _videoModel->insertRow(1);
    _videoModel->setData(_videoModel->index(1, 0), "test2.avi", CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(1, 1), 0, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(1, 2), 600, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(1, 3), 100, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->insertRow(0, _videoModel->index(1, 0));
    _videoModel->setData(_videoModel->index(0, 0, _videoModel->index(1, 0)), QLineF(60, 400, 200, 80),
                         CvmobVideoModel::VideoSceneEditRole);
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
    _videoModel->setData(fileIndex, fileName, CvmobVideoModel::VideoSceneEditRole);
    _ui->openedVideosList->setCurrentIndex(fileIndex);
}

CvMobMainWindow::~CvMobMainWindow()
{
	delete _ui;
}
