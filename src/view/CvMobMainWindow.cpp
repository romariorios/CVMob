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
	QMainWindow(parent)
{
	ui.setupUi(this);
	state = STOP;
	inputMethod=AVI;
	posSliderOld=1;

	dialogCalibration = new DialogCalibrationWidget();
	dialogCalibration->setupUi(dialogCalibration);
	imgVwr = new imageViewer;
	tableModelFixPoints = new QStandardItemModel(4, 2);
	tableModelTrajPoints = new QStandardItemModel(4, 7);
	tableModelAnglePoints = new QStandardItemModel(4, 2);
	fixPointHeader=QStringList()<<"Point"<<"Distance";
	trajPointHeader=QStringList()<<"Frame"<<"Time"<<"Vel"<< "Acce"<<"X"<<"Y"<<"VelX"<<"VelY";
	anglePointHeader=QStringList()<<"Time"<<"Angle";
	tableModelFixPoints->setHorizontalHeaderLabels(fixPointHeader);
	tableModelTrajPoints->setHorizontalHeaderLabels(trajPointHeader);
	tableModelAnglePoints->setHorizontalHeaderLabels(anglePointHeader);

    ui.gridMainLay->setColumnStretch(0,10);
    ui.gridMainLay->setColumnStretch(1,0);

    ui.tableViewFixPt->setModel(tableModelFixPoints);
    ui.tableViewTraj->setModel(tableModelTrajPoints);
    ui.tableViewAngle->setModel(tableModelAnglePoints);
    gridLayout = new QGridLayout();
	initializePlots();
	ui.GraphGroupBox->setLayout(gridLayout);
	totalFrames = 0;
	connect(ui.pushButtonStepFoward, SIGNAL(clicked()), SLOT(PressStepFoward()));
	connect(ui.pushButtonStepBack, SIGNAL(clicked()), SLOT(PressStepBack()));
	connect(ui.slider, SIGNAL(valueChanged(int)), SLOT(SliderChange(int)));
	connect(ui.pushButtonPlay, SIGNAL(clicked()), SLOT(PressPlay()));
	connect(ui.pushButtonStop, SIGNAL(clicked()), SLOT(PressStop()));
	connect(ui.pushButtonClearPoints, SIGNAL(clicked()), SLOT(FreeFixPoints()));
	connect(ui.pushButtonClearTrajectories, SIGNAL(clicked()), SLOT(freeTrajPoints()));
	connect(ui.actionOpen, SIGNAL(triggered()), SLOT(FileOpen()));
	connect(ui.actionAngle, SIGNAL(triggered()), SLOT(ExportAngle()));
	connect(ui.actionTrajectory, SIGNAL(triggered()), SLOT(ExportTrajectory()));

	// Calibration Menu
	connect(ui.actionStart_Calibration, SIGNAL(triggered()), SLOT(StartCalibration()));
	// Graphs  Menu
	connect(ui.actionVelocity, SIGNAL(triggered()), SLOT(selectVelocity()));
	connect(ui.actionAcceleration, SIGNAL(triggered()), SLOT(selectAcceleration()));
	connect(ui.actionTrabalho, SIGNAL(triggered()), SLOT(selectTrabalho()));
	connect(ui.actionX_Trajectory, SIGNAL(triggered()), SLOT(selectX_Trajectory()));
	connect(ui.actionY_Trajectory, SIGNAL(triggered()), SLOT(selectY_Trajectory()));

    // Image Viewer
    connect(imgVwr, SIGNAL(updateKeyboard(int)), SLOT(updateKeyboard(int)));
    connect(imgVwr, SIGNAL(updatWinSize(double)), SLOT(on_doubleSpinBoxWinSize_valueChanged(double)));
    connect(imgVwr, SIGNAL(closedImgView()), SLOT(on_closedImgView()));

	// About Menu
	connect(ui.actionAbout_CvMob2, SIGNAL(triggered()), SLOT(aboutClicked()));

	// Calibration Dialog
	connect(dialogCalibration->btnOK, SIGNAL(clicked()), SLOT(oKDialogCalibration()));
	connect(dialogCalibration->btnCancel, SIGNAL(clicked()), SLOT(cancelDialogCalibration()));
}

void CvMobMainWindow::MakesModelConections()
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

void CvMobMainWindow::initializePlots(){

	plots.insert(VELOCITY,new Plot(tr("Velocity"), tr("Frames") , tr("vel (pxl/frame)")));

	plots.insert(ACCELERATION,new Plot (tr("Acceleration"), tr("Frames"), tr("acc (pxl/frame²)")));

		plots.insert(TRAJ_X,new Plot (tr("x(t)"), tr("Time"), tr("x(t) (pxl)")));

		plots.insert(TRAJ_Y,new Plot (tr("y(t)"), tr("time"), tr("y(t) (pxl)")));

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
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose a file to open"),".",tr("Movie (*.avi)")).toUtf8();
        if(!filename.isEmpty())
        {
            if(!imgVwr->isHidden())
            {
                imgVwr->close();
            }
            // connect model to receive updates
            MakesModelConections();
            // Free grap
            foreach ( Plot* plot , plots){
                                plot->releaseCurves();
                                plot->replot();
                };


	    if(FacadeController::getInstance()->openVideo(filename))
            {

                        FacadeController::getInstance()->captureFrame(1);
			totalFrames = FacadeController::getInstance()->getTotalFrames();
						state=STOP;
                        ui.slider->setRange(1,totalFrames);
                        ui.slider->setValue(1);
                        imgVwr->show();
            }
	}


}

void CvMobMainWindow::ExportAngle() {

}

void CvMobMainWindow::ExportTrajectory() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Choose a name to save"),".");
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
        ui.comboBoxPoint->clear();

}

void CvMobMainWindow::CheckGrid(int checked) {

}

void CvMobMainWindow::TextGrid(const QString &text) {

}

void CvMobMainWindow::SliderChange(int i ) {
    char buff[50];
        if(totalFrames)
        {
            if(ProxyOpenCv::getInstance()->points.size()>0)
            {
                if(abs(posSliderOld-i)>1 && state!=STOP)
                {
                    int signal = (i-posSliderOld)/abs(i-posSliderOld);
                    ui.slider->setValue(posSliderOld+(1*signal));
                    return;
                }
            }
            sprintf(buff, "Frame: %d", i);
            ui.labelFrame->setText(buff);
            FacadeController::getInstance()->calculateTime(i,buff);
            ui.labelTime->setText(buff);
            FacadeController::getInstance()->captureFrame(i);
            FacadeController::getInstance()->calculateData(i);

            posSliderOld=i;
        }
}

void CvMobMainWindow::atualizeTableFixPoints() {
    ProxyOpenCv *cV;
    double d;
    cV=ProxyOpenCv::getInstance();
    if(cV->fixedPoints.size()==0)
        return;
    tableModelFixPoints->clear();
    tableModelFixPoints->setHorizontalHeaderLabels(fixPointHeader);

    for (unsigned int i = 0; i < cV->fixedPoints.size(); i++) {
        if(i%2)
        {
            d=calcDistance(cV->fixedPoints[i].markedPoint,
                                 cV->fixedPoints[i-1].markedPoint,
                                 cV->get_horizontalRazao(),cV->get_verticalRazao());
            // atualize distance text
            //item=;
            tableModelFixPoints->setItem(i/2,0,new QStandardItem(QString::number(i/2+1)));
            tableModelFixPoints->setItem(i/2,1,new QStandardItem(QString::number(d,'f',imgVwr->op.prec)));
        }
    }
    ui.tabFixedPoints->repaint();
}

void CvMobMainWindow::atualizeTableTrajPoints() {
    ProxyOpenCv *cV;
    int pt=ui.comboBoxPoint->currentIndex();
    cV=ProxyOpenCv::getInstance();
    if(cV->getCountPoints()==0)         // if there is any point, do nothing
        return;
    if(pt>cV->getCountPoints()-1) // In the case of the maximum spinBox are not atualizated
        pt=cV->getCountPoints()-1;

    tableModelTrajPoints->clear();
    tableModelTrajPoints->setHorizontalHeaderLabels(trajPointHeader);
    int iF=cV->points[pt].initFrame;
    for (int i =0 ; i < cV->points[pt].time.size(); i++) {
        // atualize distance text
            tableModelTrajPoints->setItem(i,0,new QStandardItem(QString::number(i+iF)));
            tableModelTrajPoints->setItem(i,1,new QStandardItem(QString::number(cV->points[pt].time[i],'f',imgVwr->op.prec)));
            tableModelTrajPoints->setItem(i,2,new QStandardItem(
                    QString::number(cV->points[pt].velocity[i],'f',imgVwr->op.prec)));
            tableModelTrajPoints->setItem(i,3,new QStandardItem(
                    QString::number(cV->points[pt].acceleration[i],'f',imgVwr->op.prec)));
            tableModelTrajPoints->setItem(i,4,new QStandardItem(
                    QString::number(cV->points[pt].trajectorie[i].x,'f',imgVwr->op.prec)));
            tableModelTrajPoints->setItem(i,5,new QStandardItem(
                    QString::number(cV->points[pt].trajectorie[i].y,'f',imgVwr->op.prec)));
            tableModelTrajPoints->setItem(i,6,new QStandardItem(
                    QString::number(cV->points[pt].xVelocity[i],'f',imgVwr->op.prec)));
            tableModelTrajPoints->setItem(i,7,new QStandardItem(
                    QString::number(cV->points[pt].yVelocity[i],'f',imgVwr->op.prec)));


    }
    ui.tabTrajectory->repaint();
}

void CvMobMainWindow::atualizeTableAnglePoints() {
    ProxyOpenCv *cV;
    int pt=ui.comboBoxAngle->currentIndex();
    cV=ProxyOpenCv::getInstance();
    if(cV->angles.size()==0)         // if there is any point, do nothing
        return;
    if(pt>cV->angles.size()-1) // In the case of the maximum spinBox are not atualizated
        pt=cV->angles.size()-1;

    tableModelAnglePoints->clear();
    tableModelAnglePoints->setHorizontalHeaderLabels(anglePointHeader);
    int iF=cV->angles[pt].initFrame;
    for (int i =0 ; i < cV->angles[pt].time.size(); i++) {
            tableModelAnglePoints->setItem(i,0,new QStandardItem(QString::number(cV->angles[pt].time[i],'f',imgVwr->op.prec)));
            tableModelAnglePoints->setItem(i,1,new QStandardItem(
                    QString::number(cV->angles[pt].value[i],'f',imgVwr->op.prec)));
    }
    ui.tabTrajectory->repaint();
}


double CvMobMainWindow::calcDistance(CvPoint2D32f p1,CvPoint2D32f p2,float hR,float vR)
{
    double dx=(p2.x-p1.x);
    double dy=(p2.y-p1.y);
    return sqrt(dx*dx*hR*hR+dy*dy*vR*vR);
}

void CvMobMainWindow::PressPlay() {
    if(inputMethod==CAM)
    {
        if(state==PLAY)   // same as REC
        {
            state=PAUSE;
            ui.pushButtonPlay->setText(tr("REC"));
            FacadeController::getInstance()->setRecording(false);
        }else                   // PAUSE
        {
            state=PLAY;
            ui.pushButtonPlay->setText(tr("Pause"));
            FacadeController::getInstance()->setRecording(true);
        }

    }else
    {
        if(state==PLAY)
        {
            state=PAUSE;
            ui.pushButtonPlay->setText(tr("Play"));
        }else                   // PAUSE
        {
            state = PLAY;
            ui.pushButtonPlay->setText(tr("Pause"));
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
        on_pushButtonClearAngles_clicked();
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
		message->setText(tr("Set a valid value for Calibration"));
		message->show();
	}
	dialogCalibration->close();
}


void CvMobMainWindow::calibrateOk(bool ok){
	QMessageBox *message = new QMessageBox();
	if (ok){
//		change plots to work with seconds and
		calibratePlots();
				freeTrajPoints();
        message->setText(tr("Calibration successful"));
	}else{
				message->setText(tr("Error on Calibration. Use 2 points."));
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
    if(imgVwr->geometry().height()==0)
        imgVwr->setGeometry(geometry().x()+geometry().width(),
                                 geometry().y(),image.cols,image.rows);
    imgVwr->setImage(image);
    imgVwr->repaint();
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

        if(ui.tabWidgetData->currentIndex()==0) // case first tab selected
        {
            table=ui.tableViewTraj;
            headerList=trajPointHeader;
        }else if(ui.tabWidgetData->currentIndex()==1)
        {
            table=ui.tableViewFixPt;
            headerList=fixPointHeader;
        }else
        {
            table=ui.tableViewAngle;
            headerList=anglePointHeader;
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
    updateKeyboard(e->key());
}



void CvMobMainWindow::updateKeyboard(int c ){
    switch (c) {
            case Qt::Key_Left:
                PressStepBack();
                break;
            case Qt::Key_Right:
                PressStepFoward();
                break;
            case Qt::Key_F:
                FreeFixPoints();
                break;
            case Qt::Key_T:
                freeTrajPoints();
                break;
            case Qt::Key_R:
                PressStop();
                break;
            case Qt::Key_Space:
                PressPlay();
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
	plots.at(VELOCITY)->setXAxisTitle(tr("seconds"));
	plots.at(VELOCITY)->setYAxisTitle(tr("vel (m/s)"));

	plots.at(ACCELERATION)->setXAxisTitle(tr("seconds"));
	plots.at(ACCELERATION)->setYAxisTitle(tr("acc (m/s²)"));

    plots.at(TRAJ_X)->setXAxisTitle(tr("seconds"));
    plots.at(TRAJ_X)->setYAxisTitle(tr("x(T) (m)"));

    plots.at(TRAJ_Y)->setXAxisTitle(tr("seconds"));
    plots.at(TRAJ_Y)->setYAxisTitle(tr("y(T) (m)"));
}

CvMobMainWindow::~CvMobMainWindow() {
    imgVwr->deleteLater();
}
void CvMobMainWindow::newTrajPoint(){
    QPixmap iten(10,10);
    ProxyOpenCv *cV;
    cV=ProxyOpenCv::getInstance();

    int f=cV->points.size()-1;  // The last Item
    iten.fill(QColor(cV->points[f].color[0],cV->points[f].color[1],cV->points[f].color[2]));
    ui.comboBoxPoint->addItem(QIcon(iten)," ");
}

void CvMobMainWindow::newAnglePoint(){
    ProxyOpenCv *cV;
    cV=ProxyOpenCv::getInstance();

    int f=cV->angles.size();  // The last Item
    ui.comboBoxAngle->addItem(QString::number(f)," ");
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
    ui.doubleSpinBoxWinSize->setValue(value);
}

void view::CvMobMainWindow::closeEvent(QCloseEvent *event){
    imgVwr->close();
}

void view::CvMobMainWindow::on_updateFixButton_clicked()
{
    atualizeTableFixPoints();
    ui.tableViewFixPt->resizeColumnsToContents();
}

void view::CvMobMainWindow::on_tabWidgetData_selected(QString )
{
}

void view::CvMobMainWindow::on_updateTrajButton_clicked()
{
    atualizeTableTrajPoints();
    ui.tableViewTraj->resizeColumnsToContents();

}

void view::CvMobMainWindow::on_updateAngleButton_clicked()
{
  atualizeTableAnglePoints();
  ui.tableViewAngle->resizeColumnsToContents();
}


void view::CvMobMainWindow::on_actionOpen_Cam_triggered()
{
    ui.pushButtonStepBack->setEnabled(false);
    ui.pushButtonStepFoward->setEnabled(false);
    ui.pushButtonPlay->setText(tr("REC"));
    ui.pushButtonStop->setEnabled(false);
    ui.slider->setEnabled(false);
    state=PAUSE;
    inputMethod=CAM;

    if(!imgVwr->isHidden())
        imgVwr->close();

    // connect model to receive updates
    MakesModelConections();
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
     imgVwr->show();
    }
    int ifrm=0;
    while (inputMethod==CAM)
    {
        FacadeController::getInstance()->captureFrame(ifrm);
        FacadeController::getInstance()->calculateData(ifrm);
        QApplication::processEvents();
        ifrm++;
    }


}

void view::CvMobMainWindow::on_closedImgView()
{
    ui.pushButtonStepBack->setEnabled(true);
    ui.pushButtonStepFoward->setEnabled(true);
    ui.pushButtonPlay->setText(tr("Play"));
    ui.pushButtonStop->setEnabled(true);
    ui.slider->setEnabled(true);
    state=PAUSE;
    inputMethod=AVI;

}


void view::CvMobMainWindow::on_pushButtonClearAngles_clicked()
{
    FacadeController::getInstance()->freeAnglePoints();
    ui.comboBoxAngle->clear();
}

