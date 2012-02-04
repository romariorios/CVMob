#include "cvmobmainwindow.h"
#include "ui_cvmobmainwindow.h"

#include <model/cvmobvideomodel.hpp>
#include <QtCore/QPointF>
#include <QtGui/QStandardItemModel>
#include <view/videoview.h>

#include "graphs/Plot.h"

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CvMobMainWindow),
    _videoModel(new CvmobVideoModel),
    _videoView(new VideoView)
{
    _ui->setupUi(this);

    _ui->openButton->setDefaultAction(_ui->action_Open);

    _ui->distancesView->setModel(_videoModel);
    _videoView->setModel(_videoModel);

    setCentralWidget(dynamic_cast<QGraphicsView *>(_videoView));

    connect(_ui->action_Open, SIGNAL(triggered()), SLOT(openFile()));

    _videoModel->insertRow(0);
    _videoModel->insertColumns(0, 7);
    _videoModel->setData(_videoModel->index(0, 0), "test.avi", CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, 1), 0, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, 2), 600, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, 3), 100, CvmobVideoModel::VideoSceneEditRole);
    _videoModel->insertRow(0, _videoModel->index(0, 4));
    _videoModel->insertColumns(0, 2, _videoModel->index(0, 4));
    _videoModel->setData(_videoModel->index(0, 0, _videoModel->index(0, 4)), QPointF(0, 0),
                         CvmobVideoModel::VideoSceneEditRole);
    _videoModel->setData(_videoModel->index(0, 1, _videoModel->index(0, 4)), QPointF(100, 100),
                         CvmobVideoModel::VideoSceneEditRole);
}

void CvMobMainWindow::openFile() {

}

CvMobMainWindow::~CvMobMainWindow()
{
	delete _ui;
}
