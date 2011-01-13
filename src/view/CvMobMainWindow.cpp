/*
 * CvMobMainWindow.cpp
 *
 *  Created on: 19/08/2008
 *      Author: paulista
 */

#include "CvMobMainWindow.h"
#include <QWidget>
#include <cassert>


#include <iostream>

using namespace std;

using namespace controller;

namespace view {

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
	QMainWindow(parent) {
	ui.setupUi(this);
	state = STOP;
        this->inputMethod=AVI;
	this->posSliderOld=1;

	dialogCalibration = new DialogCalibrationWidget();
        dialogCalibration->setupUi(dialogCalibration);
        this->imgVwr= new imageViewer;
        this->tableModelFixPoints = new QStandardItemModel(4, 2);
        this->tableModelTrajPoints = new QStandardItemModel(4, 7);
        this->tableModelAnglePoints = new QStandardItemModel(4, 2);
        this->fixPointHeader=QStringList()<<"Point"<<"Distance";
        this->trajPointHeader=QStringList()<<"Frame"<<"Time"<<"Vel"<< "Acce"<<"X"<<"Y"<<"VelX"<<"VelY";
        this->anglePointHeader=QStringList()<<"Time"<<"Angle";
        this->tableModelFixPoints->setHorizontalHeaderLabels(this->fixPointHeader);
        this->tableModelTrajPoints->setHorizontalHeaderLabels(this->trajPointHeader);
        this->tableModelAnglePoints->setHorizontalHeaderLabels(this->anglePointHeader);


        this->ui.gridMainLay->setColumnStretch(0,10);
        this->ui.gridMainLay->setColumnStretch(1,0);

        this->ui.tableViewFixPt->setModel(this->tableModelFixPoints);
        this->ui.tableViewTraj->setModel(this->tableModelTrajPoints);
        this->ui.tableViewAngle->setModel(this->tableModelAnglePoints);
        gridLayout = new QGridLayout();
	initializePlots();
	ui.GraphGroupBox->setLayout(gridLayout);
	totalFrames = 0;
	connect(ui.pushButtonStepFoward, SIGNAL(clicked()), this, SLOT(PressStepFoward()));
	connect(ui.pushButtonStepBack, SIGNAL(clicked()), this, SLOT(PressStepBack()));
	connect(ui.slider, SIGNAL(valueChanged(int)), this, SLOT(SliderChange(int)));
	connect(ui.pushButtonPlay, SIGNAL(clicked()), this, SLOT(PressPlay()));
	connect(ui.pushButtonStop, SIGNAL(clicked()), this, SLOT(PressStop()));
        connect(ui.pushButtonClearPoints, SIGNAL(clicked()), this, SLOT(FreeFixPoints()));
        connect(ui.pushButtonClearTrajectories, SIGNAL(clicked()), this, SLOT(freeTrajPoints()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(FileOpen()));
	connect(ui.actionAngle, SIGNAL(triggered()), this, SLOT(ExportAngle()));
	connect(ui.actionTrajectory, SIGNAL(triggered()), this, SLOT(ExportTrajectory()));

	// Calibration Menu
	connect(ui.actionStart_Calibration, SIGNAL(triggered()), this, SLOT(StartCalibration()));
        // Graphs  Menu
        connect(ui.actionVelocity, SIGNAL(triggered()),this, SLOT(selectVelocity()));
	connect(ui.actionAcceleration, SIGNAL(triggered()),this, SLOT(selectAcceleration()));
	connect(ui.actionTrabalho, SIGNAL(triggered()),this,SLOT(selectTrabalho()));
        connect(ui.actionX_Trajectory, SIGNAL(triggered()),this,SLOT(selectX_Trajectory()));
        connect(ui.actionY_Trajectory, SIGNAL(triggered()),this,SLOT(selectY_Trajectory()));

        // Image Viewer
        connect( imgVwr, SIGNAL(updateKeyboard(int)),this,SLOT(updateKeyboard(int)));
        connect( imgVwr, SIGNAL(updatWinSize(double)),this,SLOT(on_doubleSpinBoxWinSize_valueChanged(double )));
        connect( imgVwr, SIGNAL(closedImgView()),this,SLOT(on_closedImgView()));



	// About Menu

	connect(ui.actionAbout_CvMob2, SIGNAL(triggered()), this, SLOT(aboutClicked()));


	// Calibration Dialog
	connect(dialogCalibration->btnOK, SIGNAL(clicked()),this, SLOT(oKDialogCalibration()));
	connect(dialogCalibration->btnCancel, SIGNAL(clicked()),this, SLOT(cancelDialogCalibration()));

	// connect model to receive updates
        //Only when open the file



}
void CvMobMainWindow::MakesModelConections()
{
    // connect model to receive updates
    connect(ProxyOpenCv::getInstance(),SIGNAL(calibrateOk(bool)),this,SLOT(calibrateOk(bool)));
    connect(ProxyOpenCv::getInstance(),SIGNAL(addCurve(int, int , int, int)),this,SLOT(addCurve(int, int, int, int)));
    connect( ProxyOpenCv::getInstance(), SIGNAL(updateVelocity(int, QVector <double>, QVector <double>, int)),this,SLOT(updateVelocity(int, QVector <double>, QVector <double>, int)));
    connect( ProxyOpenCv::getInstance(), SIGNAL(updateAcceleration(int, QVector <double>, QVector <double>, int)),this,SLOT(updateAcceleration(int, QVector <double>, QVector <double>, int)));
    connect( ProxyOpenCv::getInstance(), SIGNAL(updateWork(int, QVector <double>, QVector <double>, int)),this,SLOT(updateWork(int, QVector <double>, QVector <double>, int)));
    connect( ProxyOpenCv::getInstance(), SIGNAL(updateTraj_x(int, QVector <double>, QVector <double>, int)),this,SLOT(updateTraj_x(int, QVector <double>, QVector <double>, int)));
    connect( ProxyOpenCv::getInstance(), SIGNAL(updateTraj_y(int, QVector <double>, QVector <double>, int)),this,SLOT(updateTraj_y(int, QVector <double>, QVector <double>, int)));
    connect( ProxyOpenCv::getInstance(), SIGNAL(updateImage(Mat )),this,SLOT(updateImage(Mat )));
    connect( ProxyOpenCv::getInstance(), SIGNAL(newTrajPoint()),this,SLOT(newTrajPoint()));
    connect( ProxyOpenCv::getInstance(), SIGNAL(newAnglePoint()),this,SLOT(newAnglePoint()));

}

void CvMobMainWindow::initializePlots(){

	plots.insert(VELOCITY,new Plot("Velocity","Frames" ,"vel (pxl/frame)"));

	plots.insert(ACCELERATION,new Plot ("Acceleration", "Frames", "acc (pxl/frame�)"));

        plots.insert(TRAJ_X,new Plot ("x(t)", "time", "x(t) (pxl)"));

        plots.insert(TRAJ_Y,new Plot ("y(t)", "time", "y(t) (pxl)"));

       // plots.insert(WORK,new Plot ("Work/Mass", "Frames", "work ((pxl�/frame�))"));

	foreach(Plot* plot, plots){
		plot->setAutoReplot(true);
		plot->hide();
		gridLayout->addWidget(plot);
	}

//	show this two graphs.
	plots.at(VELOCITY)->show();
	plots.at(ACCELERATION)->show();
//        plots.at(TRAJ_X)->show();


}

void CvMobMainWindow::FileOpen() {
	QString filename = QFileDialog::getOpenFileName(this,"Choose a file to open",".","Movie (*.avi)").toUtf8();
        if(!filename.isEmpty())
        {
            if(!this->imgVwr->isHidden())
            {
                this->imgVwr->close();
            }
            // connect model to receive updates
            this->MakesModelConections();
            // Free grap
            foreach ( Plot* plot , plots){
                                plot->releaseCurves();
                                plot->replot();
                };


	    if(FacadeController::getInstance()->openVideo(filename))
            {

                        FacadeController::getInstance()->captureFrame(1);
			totalFrames = FacadeController::getInstance()->getTotalFrames();
                        this->state=STOP;
                        ui.slider->setRange(1,totalFrames);
                        ui.slider->setValue(1);
                        this->imgVwr->show();
            }
	}


}

void CvMobMainWindow::ExportAngle() {

}

void CvMobMainWindow::ExportTrajectory() {
	QString filename = QFileDialog::getSaveFileName(this,"Choose a name to save",".");
	if(!filename.isEmpty()){
		FacadeController::getInstance()->exportTrajectory(filename.toAscii().data());
	}
}

void CvMobMainWindow::PressStepFoward() {
	if(ui.slider->value() < ui.slider->maximum())
		ui.slider->setValue(ui.slider->value()+1);
}

void CvMobMainWindow::PressStepBack() {
        if (ui.slider->value() > 1)
		ui.slider->setValue(ui.slider->value()-1);
}

void CvMobMainWindow::FreeFixPoints() {

        FacadeController::getInstance()->freeFixPoints();
}

void CvMobMainWindow::freeTrajPoints() {
    foreach ( Plot* plot , plots){
                    plot->releaseCurves();
                    plot->replot();
    };
        FacadeController::getInstance()->freeTrajPoints();
        this->ui.comboBoxPoint->clear();

}

void CvMobMainWindow::CheckGrid(int checked) {

}

void CvMobMainWindow::TextGrid(const QString &text) {

}

void CvMobMainWindow::SliderChange(int i ) {
	char buff[50];
        if(this->totalFrames)
        {
            if(ProxyOpenCv::getInstance()->points.size()>0)
            {
                if(abs(this->posSliderOld-i)>1 && this->state!=STOP)
                {
                    int signal = (i-this->posSliderOld)/abs(i-this->posSliderOld);
                    this->ui.slider->setValue(this->posSliderOld+(1*signal));
                    return;
                }
            }
            sprintf(buff, "Frame: %d", i);
            ui.labelFrame->setText(buff);
            FacadeController::getInstance()->calculateTime(i,buff);
            ui.labelTime->setText(buff);
            FacadeController::getInstance()->captureFrame(i);
            FacadeController::getInstance()->calculateData(i);

            this->posSliderOld=i;
        }
}

void CvMobMainWindow::atualizeTableFixPoints() {
    ProxyOpenCv *cV;
    double d;
    cV=ProxyOpenCv::getInstance();
    if(cV->fixedPoints.size()==0)
        return;
    this->tableModelFixPoints->clear();
    this->tableModelFixPoints->setHorizontalHeaderLabels(this->fixPointHeader);

    for (unsigned int i = 0; i < cV->fixedPoints.size(); i++) {
        if(i%2)
        {
            d=this->calcDistance(cV->fixedPoints[i].markedPoint,
                                 cV->fixedPoints[i-1].markedPoint,
                                 cV->get_horizontalRazao(),cV->get_verticalRazao());
            // atualize distance text
            //item=;
            this->tableModelFixPoints->setItem(i/2,0,new QStandardItem(QString::number(i/2+1)));
            this->tableModelFixPoints->setItem(i/2,1,new QStandardItem(QString::number(d,'f',this->imgVwr->op.prec)));
        }
    }
    this->ui.tabFixedPoints->repaint();
}

void CvMobMainWindow::atualizeTableTrajPoints() {
    ProxyOpenCv *cV;
    int pt=this->ui.comboBoxPoint->currentIndex();
    cV=ProxyOpenCv::getInstance();
    if(cV->getCountPoints()==0)         // if there is any point, do nothing
        return;
    if(pt>cV->getCountPoints()-1) // In the case of the maximum spinBox are not atualizated
        pt=cV->getCountPoints()-1;

    this->tableModelTrajPoints->clear();
    this->tableModelTrajPoints->setHorizontalHeaderLabels(this->trajPointHeader);
    int iF=cV->points[pt].initFrame;
    for (int i =0 ; i < cV->points[pt].time.size(); i++) {
        // atualize distance text
            this->tableModelTrajPoints->setItem(i,0,new QStandardItem(QString::number(i+iF)));
            this->tableModelTrajPoints->setItem(i,1,new QStandardItem(QString::number(cV->points[pt].time[i],'f',this->imgVwr->op.prec)));
            this->tableModelTrajPoints->setItem(i,2,new QStandardItem(
                    QString::number(cV->points[pt].velocity[i],'f',this->imgVwr->op.prec)));
            this->tableModelTrajPoints->setItem(i,3,new QStandardItem(
                    QString::number(cV->points[pt].acceleration[i],'f',this->imgVwr->op.prec)));
            this->tableModelTrajPoints->setItem(i,4,new QStandardItem(
                    QString::number(cV->points[pt].trajectorie[i].x,'f',this->imgVwr->op.prec)));
            this->tableModelTrajPoints->setItem(i,5,new QStandardItem(
                    QString::number(cV->points[pt].trajectorie[i].y,'f',this->imgVwr->op.prec)));
            this->tableModelTrajPoints->setItem(i,6,new QStandardItem(
                    QString::number(cV->points[pt].xVelocity[i],'f',this->imgVwr->op.prec)));
            this->tableModelTrajPoints->setItem(i,7,new QStandardItem(
                    QString::number(cV->points[pt].yVelocity[i],'f',this->imgVwr->op.prec)));


    }
    this->ui.tabTrajectory->repaint();
}

void CvMobMainWindow::atualizeTableAnglePoints() {
    ProxyOpenCv *cV;
    int pt=this->ui.comboBoxAngle->currentIndex();
    cV=ProxyOpenCv::getInstance();
    if(cV->angles.size()==0)         // if there is any point, do nothing
        return;
    if(pt>cV->angles.size()-1) // In the case of the maximum spinBox are not atualizated
        pt=cV->angles.size()-1;

    this->tableModelAnglePoints->clear();
    this->tableModelAnglePoints->setHorizontalHeaderLabels(this->anglePointHeader);
    int iF=cV->angles[pt].initFrame;
    for (int i =0 ; i < cV->angles[pt].time.size(); i++) {
            this->tableModelAnglePoints->setItem(i,0,new QStandardItem(QString::number(cV->angles[pt].time[i],'f',this->imgVwr->op.prec)));
            this->tableModelAnglePoints->setItem(i,1,new QStandardItem(
                    QString::number(cV->angles[pt].value[i],'f',this->imgVwr->op.prec)));
    }
    this->ui.tabTrajectory->repaint();
}


double CvMobMainWindow::calcDistance(CvPoint2D32f p1,CvPoint2D32f p2,float hR,float vR)
{
    double dx=(p2.x-p1.x);
    double dy=(p2.y-p1.y);
    return sqrt(dx*dx*hR*hR+dy*dy*vR*vR);
}

void CvMobMainWindow::PressPlay() {
    if(this->inputMethod==CAM)
    {
        if(this->state==PLAY)   // same as REC
        {
            state=PAUSE;
            ui.pushButtonPlay->setText("REC");
            FacadeController::getInstance()->setRecording(false);
        }else                   // PAUSE
        {
            state=PLAY;
            ui.pushButtonPlay->setText("Pause");
            FacadeController::getInstance()->setRecording(true);
        }

    }else
    {
        if(this->state==PLAY)
        {
            state=PAUSE;
            ui.pushButtonPlay->setText("Play");
        }else                   // PAUSE
        {
            state = PLAY;
            ui.pushButtonPlay->setText("Pause");
            for(int i= ui.slider->value()+1; i < ui.slider->maximum(); i++) {
                    if (state == PLAY)
                    {
                            ui.slider->setValue(i);
                            QApplication::processEvents();
                    }
                    else {
                            break;
                    }
            }
        }
    }

}

void CvMobMainWindow::PressStop() {
	state = STOP;
        FreeFixPoints();
        freeTrajPoints();
        this->on_pushButtonClearAngles_clicked();
        ui.slider->setValue(1);
	QApplication::processEvents();
}

void CvMobMainWindow::StartCalibration(){
	dialogCalibration->show();
}

void CvMobMainWindow::oKDialogCalibration(){
        double Distance = dialogCalibration->txtDistance->text().trimmed().toFloat();
        if (Distance > 0)
                FacadeController::getInstance()->startCalibration(Distance);
	else{
		QMessageBox *message = new QMessageBox();
		message->setText("Set a valid value for Calibration");
		message->show();
	}
	dialogCalibration->close();
}


void CvMobMainWindow::calibrateOk(bool ok){
	QMessageBox *message = new QMessageBox();
	if (ok){
//		change plots to work with seconds and
		calibratePlots();
                this->freeTrajPoints();
		message->setText("Calibration successful");
	}else{
                message->setText("Error on Calibration. Use 2 points.");
	}
	message->show();
}

void CvMobMainWindow::cancelDialogCalibration(){
	dialogCalibration->close();
}


void CvMobMainWindow::addCurve(int index, int r, int g, int b){
        foreach(Plot* plot, plots){
		plot->addCurve(index, r, g, b);
	}
}

void CvMobMainWindow::updateVelocity(int index, QVector <double> time, QVector <double> velocity, int actualTime ){
    plots.at(VELOCITY)->setData(index,time.data()+2,velocity.data()+2,velocity.size()-2);
}

void CvMobMainWindow::updateAcceleration(int index, QVector <double> time, QVector <double> acceleration, int actualTime ){
    plots.at(ACCELERATION)->setData(index,time.data()+4,acceleration.data()+4,acceleration.size()-4);
}

void CvMobMainWindow::updateWork(int index, QVector <double> time, QVector <double> work, int actualTime ){
    plots.at(WORK)->setData(index,time.data(),work.data(),work.size());
}

void CvMobMainWindow::updateTraj_x(int index, QVector <double> time, QVector <double> traj_x, int actualTime ){
    plots.at(TRAJ_X)->setData(index,time.data(),traj_x.data(),traj_x.size());
}

void CvMobMainWindow::updateTraj_y(int index, QVector <double> time, QVector <double> traj_y, int actualTime ){
    plots.at(TRAJ_Y)->setData(index,time.data(),traj_y.data(),traj_y.size());
}

void CvMobMainWindow::updateImage(Mat image ){
    if(this->imgVwr->geometry().height()==0)
        this->imgVwr->setGeometry(geometry().x()+geometry().width(),
                                 geometry().y(),image.cols,image.rows);
    this->imgVwr->setImage(image);
    this->imgVwr->repaint();
}



void CvMobMainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e==QKeySequence::Copy){
        QString strClipBoard;
        QClipboard *clipboard = QApplication::clipboard();
        QItemSelectionModel *selectionModel;
        QTableView *table;
        QStringList headerList;
        int r,c;

        if(this->ui.tabWidgetData->currentIndex()==0) // case first tab selected
        {
            table=this->ui.tableViewTraj;
            headerList=this->trajPointHeader;
        }else if(this->ui.tabWidgetData->currentIndex()==1)
        {
            table=this->ui.tableViewFixPt;
            headerList=this->fixPointHeader;
        }else
        {
            table=this->ui.tableViewAngle;
            headerList=this->anglePointHeader;
        }
        selectionModel = table->selectionModel();
        if(selectionModel->hasSelection())
        {
            QModelIndexList itemList = selectionModel->selectedColumns();
            if(itemList.size()) // If there is any column entired selected.
            {
                // include header in the clipBoard
                foreach(QModelIndex iL,itemList){
                    c=iL.column();
                    strClipBoard.append(headerList.at(c)+"\t");
                }
                strClipBoard.append("\n");
                // Put data in the ClipBoard
                for(r=0;r<table->model()->rowCount();r++)
                {
                    foreach(QModelIndex iL,itemList){
                        c=iL.column();
                        strClipBoard.append(table->model()->index(r,c).data().toString()+"\t");
                    }
                    strClipBoard.append("\n");
                }
            }


        }

        clipboard->setText(strClipBoard);
    }
    this->updateKeyboard(e->key());
}



void CvMobMainWindow::updateKeyboard(int c ){
    switch (c) {
            case Qt::Key_Left:
                this->PressStepBack();
                break;
            case Qt::Key_Right:
                this->PressStepFoward();
                break;
            case Qt::Key_F:
                this->FreeFixPoints();
                break;
            case Qt::Key_T:
                this->freeTrajPoints();
                break;
            case Qt::Key_R:
                this->PressStop();
                break;
            case Qt::Key_Space:
                this->PressPlay();
                break;

    }

}

void CvMobMainWindow::selectVelocity(){
	// this if is mix because the cheked event came first
	if (ui.actionVelocity->isChecked()){
		plots.at(VELOCITY)->show();
	}
	else{
		plots.at(VELOCITY)->hide();
	}
	gridLayout->update();
}

void CvMobMainWindow::selectAcceleration(){
	// this if is mix because the cheked event came first
	if (ui.actionAcceleration->isChecked()){
		plots.at(ACCELERATION)->show();
	}
	else{
		plots.at(ACCELERATION)->hide();
	}
	gridLayout->update();
}

void CvMobMainWindow::selectTrabalho(){
	// this if is mix because the cheked event came first
	if (ui.actionTrabalho->isChecked()){
		plots.at(WORK)->show();
	}
	else{
		plots.at(WORK)->hide();
	}
	gridLayout->update();
}

void CvMobMainWindow::selectX_Trajectory(){
        // this if is mix because the cheked event came first
        if (ui.actionX_Trajectory->isChecked()){
                plots.at(TRAJ_X)->show();
        }
        else{
                plots.at(TRAJ_X)->hide();
        }
        gridLayout->update();
}

void CvMobMainWindow::selectY_Trajectory(){
        // this if is mix because the cheked event came first
        if (ui.actionY_Trajectory->isChecked()){
                plots.at(TRAJ_Y)->show();
        }
        else{
                plots.at(TRAJ_Y)->hide();
        }
        gridLayout->update();
}


void CvMobMainWindow::aboutClicked(){
	AboutDialogWidget *dialog = new AboutDialogWidget();
	dialog->setupUi(dialog);
	dialog->show();
}

void CvMobMainWindow::calibratePlots(){
	plots.at(VELOCITY)->setXAxisTitle("seconds");
	plots.at(VELOCITY)->setYAxisTitle("vel (m/s)");

	plots.at(ACCELERATION)->setXAxisTitle("seconds");
	plots.at(ACCELERATION)->setYAxisTitle("acc (m/s�)");

        plots.at(TRAJ_X)->setXAxisTitle("seconds");
        plots.at(TRAJ_X)->setYAxisTitle("x(T) (m)");

        plots.at(TRAJ_Y)->setXAxisTitle("seconds");
        plots.at(TRAJ_Y)->setYAxisTitle("y(T) (m)");

        //plots.at(WORK)->setXAxisTitle("seconds");
        //plots.at(WORK)->setYAxisTitle("work (J)");
}

CvMobMainWindow::~CvMobMainWindow() {
    this->imgVwr->deleteLater();
}
void CvMobMainWindow::newTrajPoint(){
    QPixmap iten(10,10);
    ProxyOpenCv *cV;
    cV=ProxyOpenCv::getInstance();

    int f=cV->points.size()-1;  // The last Item
    iten.fill(QColor(cV->points[f].color[0],cV->points[f].color[1],cV->points[f].color[2]));
    this->ui.comboBoxPoint->addItem(QIcon(iten)," ");
}

void CvMobMainWindow::newAnglePoint(){
    ProxyOpenCv *cV;
    cV=ProxyOpenCv::getInstance();

    int f=cV->angles.size();  // The last Item
    this->ui.comboBoxAngle->addItem(QString::number(f)," ");
}


}

void view::CvMobMainWindow::on_groupBoxVectors_toggled(bool value)
{
    char type;
    if(ui.radioButtonAcceleration->isChecked()) type=2;
    else type =1;
    FacadeController::getInstance()->setShowVectors(value,type);
}

void view::CvMobMainWindow::on_radioButtonAcceleration_toggled(bool checked)
{
    FacadeController::getInstance()->setShowVectors(true,checked?2:1);
}

void view::CvMobMainWindow::on_actionReport_triggered()
{
    FacadeController::getInstance()->exportReport();

}

void view::CvMobMainWindow::on_doubleSpinBoxWinSize_valueChanged(double value)
{
    FacadeController::getInstance()->setWinSize(value);
    this->ui.doubleSpinBoxWinSize->setValue(value);
}

void view::CvMobMainWindow::closeEvent(QCloseEvent *event){
    this->imgVwr->close();
}

void view::CvMobMainWindow::on_updateFixButton_clicked()
{
    this->atualizeTableFixPoints();
    this->ui.tableViewFixPt->resizeColumnsToContents();
}

void view::CvMobMainWindow::on_tabWidgetData_selected(QString )
{
}

void view::CvMobMainWindow::on_updateTrajButton_clicked()
{
    this->atualizeTableTrajPoints();
    this->ui.tableViewTraj->resizeColumnsToContents();

}

void view::CvMobMainWindow::on_updateAngleButton_clicked()
{
  this->atualizeTableAnglePoints();
  this->ui.tableViewAngle->resizeColumnsToContents();
}


void view::CvMobMainWindow::on_actionOpen_Cam_triggered()
{
    this->ui.pushButtonStepBack->setEnabled(false);
    this->ui.pushButtonStepFoward->setEnabled(false);
    this->ui.pushButtonPlay->setText("REC");
    this->ui.pushButtonStop->setEnabled(false);
    this->ui.slider->setEnabled(false);
    this->state=PAUSE;
    this->inputMethod=CAM;

    if(!this->imgVwr->isHidden())
        this->imgVwr->close();

    // connect model to receive updates
    this->MakesModelConections();
    // Free graph
    foreach ( Plot* plot , plots){
                        plot->releaseCurves();
                        plot->replot();
        };

    if(FacadeController::getInstance()->openCam())
    {
     FacadeController::getInstance()->captureFrame(1);
     FacadeController::getInstance()->setVideoStreamType(CAM);
     totalFrames = 0;
     this->imgVwr->show();
    }
    int ifrm=0;
    while (this->inputMethod==CAM)
    {
        FacadeController::getInstance()->captureFrame(ifrm);
        FacadeController::getInstance()->calculateData(ifrm);
        QApplication::processEvents();
        ifrm++;
    }


}

void view::CvMobMainWindow::on_closedImgView()
{
    this->ui.pushButtonStepBack->setEnabled(true);
    this->ui.pushButtonStepFoward->setEnabled(true);
    this->ui.pushButtonPlay->setText("Play");
    this->ui.pushButtonStop->setEnabled(true);
    this->ui.slider->setEnabled(true);
    this->state=PAUSE;
    this->inputMethod=AVI;

}


void view::CvMobMainWindow::on_pushButtonClearAngles_clicked()
{
    FacadeController::getInstance()->freeAnglePoints();
    this->ui.comboBoxAngle->clear();
}

