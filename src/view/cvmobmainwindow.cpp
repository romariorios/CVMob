#include "cvmobmainwindow.h"
#include "ui_cvmobmainwindow.h"

#include "graphs/Plot.h"
#include "controller/FacadeController.h"
#include "model/fixedpointsdistanceslistmodel.h"
#include "view/imageviewer.h"

#include <QStandardItemModel>

using namespace model;
using namespace controller;

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CvMobMainWindow),
    _fixedPointsDistancesModel(new FixedPointsDistancesListModel(this)),
    _tableModelAnglePoints(new QStandardItemModel(this)),
    _tableModelTrajPoints(new QStandardItemModel(this)),
    _imageViewer(new imageViewer)
{
    _ui->setupUi(this);
    _ui->centralLayout->insertWidget(0, _imageViewer);

    _ui->playPauseButton->setDefaultAction(_ui->actionPlay_Pause);
    _ui->openButton->setDefaultAction(_ui->action_Open);

    _ui->fixedPointsTableView->setModel(_fixedPointsDistancesModel);

    initializePlots();
    MakesModelConnections();

    connect(_ui->action_Open, SIGNAL(triggered()), SLOT(openFile()));
}

void CvMobMainWindow::initializePlots()
{
	_plots << new Plot(tr("Velocity"), tr("Frames") , tr("vel (pxl/frame)"))
		   << new Plot(tr("Acceleration"), tr("Frames"), tr("acc (pxl/frame²)"))
		   << new Plot(tr("x(t)"), tr("Time"), tr("x(t) (pxl)"))
		   << new Plot(tr("y(t)"), tr("time"), tr("y(t) (pxl)"));

	foreach (Plot *plot, _plots) {
		plot->setAutoReplot(true);
		plot->hide();
		_ui->graphsLayout->addWidget(plot);
	}

	connect(_ui->velocityCheckBox, SIGNAL(toggled(bool)), _plots.at(0), SLOT(setShown(bool)));
	connect(_ui->accelerationCheckBox, SIGNAL(toggled(bool)), _plots.at(1), SLOT(setShown(bool)));
	connect(_ui->XCheckBox, SIGNAL(toggled(bool)), _plots.at(2), SLOT(setShown(bool)));
	connect(_ui->YCheckBox, SIGNAL(toggled(bool)), _plots.at(3), SLOT(setShown(bool)));

	_ui->velocityCheckBox->toggle();
	_ui->accelerationCheckBox->toggle();
}

void CvMobMainWindow::atualizeTableTrajPoints() {
    ProxyOpenCv *cV;
    int pt=_ui->comboBoxPoint->currentIndex();
    cV=ProxyOpenCv::getInstance();
    if(cV->getCountPoints()==0)         // if there is any point, do nothing
        return;
    if(pt>cV->getCountPoints()-1) // In the case of the maximum spinBox are not atualizated
        pt=cV->getCountPoints()-1;

    _tableModelTrajPoints->clear();
//    _tableModelTrajPoints->setHorizontalHeaderLabels(trajPointHeader); // TODO: MAKE CUSTOM MODELS FOR EACH TABLE!
    int iF=cV->points[pt].initFrame;
    for (int i =0 ; i < cV->points[pt].time.size(); i++) {
        // atualize distance text
            _tableModelTrajPoints->setItem(i,0,new QStandardItem(QString::number(i+iF)));
            _tableModelTrajPoints->setItem(i,1,new QStandardItem(QString::number(cV->points[pt].time[i],'f',_imageViewer->op.prec)));
            _tableModelTrajPoints->setItem(i,2,new QStandardItem(
                    QString::number(cV->points[pt].velocity[i],'f',_imageViewer->op.prec)));
            _tableModelTrajPoints->setItem(i,3,new QStandardItem(
                    QString::number(cV->points[pt].acceleration[i],'f',_imageViewer->op.prec)));
            _tableModelTrajPoints->setItem(i,4,new QStandardItem(
                    QString::number(cV->points[pt].trajectorie[i].x,'f',_imageViewer->op.prec)));
            _tableModelTrajPoints->setItem(i,5,new QStandardItem(
                    QString::number(cV->points[pt].trajectorie[i].y,'f',_imageViewer->op.prec)));
            _tableModelTrajPoints->setItem(i,6,new QStandardItem(
                    QString::number(cV->points[pt].xVelocity[i],'f',_imageViewer->op.prec)));
            _tableModelTrajPoints->setItem(i,7,new QStandardItem(
                    QString::number(cV->points[pt].yVelocity[i],'f',_imageViewer->op.prec)));


    }
    _ui->tabTrajectory->repaint();
}

void CvMobMainWindow::atualizeTableAnglePoints() {
    ProxyOpenCv *cV;
    int pt=_ui->comboBoxAngle->currentIndex();
    cV=ProxyOpenCv::getInstance();
    if(cV->angles.size()==0)         // if there is any point, do nothing
        return;
    if(pt>cV->angles.size()-1) // In the case of the maximum spinBox are not atualizated
        pt=cV->angles.size()-1;

    _tableModelAnglePoints->clear();
//    _tableModelAnglePoints->setHorizontalHeaderLabels(anglePointHeader); // TODO: MAKE CUSTOM MODELS FOR EACH TABLE!
    int iF=cV->angles[pt].initFrame;
    for (int i =0 ; i < cV->angles[pt].time.size(); i++) {
            _tableModelAnglePoints->setItem(i,0,new QStandardItem(QString::number(cV->angles[pt].time[i],'f',_imageViewer->op.prec)));
            _tableModelAnglePoints->setItem(i,1,new QStandardItem(
                    QString::number(cV->angles[pt].value[i],'f',_imageViewer->op.prec)));
    }
    _ui->tabTrajectory->repaint();
}

void CvMobMainWindow::MakesModelConnections()
{
    // connect model to receive updates
    connect(ProxyOpenCv::getInstance(), SIGNAL(calibrateOk(bool)), this, SLOT(calibrateOk(bool)));
    connect(ProxyOpenCv::getInstance(), SIGNAL(addCurve(int, int , int, int)),this,SLOT(addCurve(int, int, int, int)));
    connect(ProxyOpenCv::getInstance(), SIGNAL(updateVelocity(int, QVector <double>, QVector <double>, int)),this,SLOT(updateVelocity(int, QVector <double>, QVector <double>, int)));
    connect(ProxyOpenCv::getInstance(), SIGNAL(updateAcceleration(int, QVector <double>, QVector <double>, int)),this,SLOT(updateAcceleration(int, QVector <double>, QVector <double>, int)));
    connect(ProxyOpenCv::getInstance(), SIGNAL(updateWork(int, QVector <double>, QVector <double>, int)),this,SLOT(updateWork(int, QVector <double>, QVector <double>, int)));
    connect(ProxyOpenCv::getInstance(), SIGNAL(updateTraj_x(int, QVector <double>, QVector <double>, int)),this,SLOT(updateTraj_x(int, QVector <double>, QVector <double>, int)));
    connect(ProxyOpenCv::getInstance(), SIGNAL(updateTraj_y(int, QVector <double>, QVector <double>, int)),this,SLOT(updateTraj_y(int, QVector <double>, QVector <double>, int)));
    connect(ProxyOpenCv::getInstance(), SIGNAL(updateImage(Mat )),this,SLOT(updateImage(Mat )));
    connect(ProxyOpenCv::getInstance(), SIGNAL(newTrajPoint()),this,SLOT(newTrajPoint()));
    connect(ProxyOpenCv::getInstance(), SIGNAL(newAnglePoint()),this,SLOT(newAnglePoint()));
}

void CvMobMainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose a file to open"),".",tr("Movie (*.avi)")).toUtf8();

    if (!filename.isEmpty()) {
        if (!_imageViewer->isHidden()) {
            _imageViewer->close();
        }

        // connect model to receive updates
        MakesModelConnections();

        // Free grap
        foreach (Plot* plot, _plots) {
            plot->releaseCurves();
            plot->replot();
        }

        if (FacadeController::getInstance()->openVideo(filename)) {
            FacadeController::getInstance()->captureFrame(1);

            // TODO: group video player in a separate class.
            _totalFrames = FacadeController::getInstance()->getTotalFrames();
//            state=STOP;
            _ui->videoProgress->setRange(1, _totalFrames);
            _ui->videoProgress->setValue(1);
            _imageViewer->show();
        }
    }
}

CvMobMainWindow::~CvMobMainWindow()
{
	delete _ui;
}
