#include "cvmobmainwindow.h"
#include "ui_cvmobmainwindow.h"

#include <model/cvmobmodel.hpp>
#include <QtGui/QStandardItemModel>
#include <view/videoview.h>

#include "graphs/Plot.h"

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CvMobMainWindow),
    _distancesModel(new CvmobModel),
    _videoView(new VideoView)
{
    _ui->setupUi(this);

    _ui->openButton->setDefaultAction(_ui->action_Open);

    _ui->distancesView->setModel(_distancesModel);
    _videoView->setModel(_distancesModel);

    setCentralWidget(_videoView);

    connect(_ui->action_Open, SIGNAL(triggered()), SLOT(openFile()));
}

void CvMobMainWindow::openFile() {

}

CvMobMainWindow::~CvMobMainWindow()
{
	delete _ui;
}
