/*
 * CvMobMainWindow.h
 *
 *  Created on: 19/08/2008
 *      Author: paulista
 */

#ifndef CVMOBMAINWINDOW_H_
#define CVMOBMAINWINDOW_H_

#include <QtGui>
#include <qlist.h>
#include "ui_FormCvMob.h"
#include "../../ui_DialogCalibration.h" // Dont forget to include ": public QDialog" in the class definition
#include "../../ui_aboutDialog.h"
#include "graphs/Plot.h"
#include "../controller/FacadeController.h"
#include "graphs/EnumGraphs.h"
#include "imageviewer.h"

using namespace Ui;

namespace view {

class CvMobMainWindow : public QMainWindow {

Q_OBJECT
public:
	CvMobMainWindow(QWidget *parent = 0);
	void calibratePlots();
	~CvMobMainWindow();
private:
	cvMob2Class ui;
	DialogCalibration *dialogCalibration;
        imageViewer *imgVwr;
	QGridLayout *gridLayout;
	QList<Plot*> plots;
        enum status {PLAY, PAUSE, STOP};
        int inputMethod;            // AVI or CAM
	int state;
	int totalFrames;
	void initializePlots();
        QStandardItemModel *tableModelFixPoints;
        QStandardItemModel *tableModelAnglePoints;
        QStandardItemModel *tableModelTrajPoints;
        void atualizeTableFixPoints();
        void atualizeTableAnglePoints();
        void atualizeTableTrajPoints();
        double calcDistance(CvPoint2D32f p1,CvPoint2D32f p2,float hR,float vR);
        void MakesModelConections();
        QStringList fixPointHeader;
        QStringList trajPointHeader;
        QStringList anglePointHeader;

        int posSliderOld;

private slots:
        void on_updateAngleButton_clicked();
        void on_pushButtonClearAngles_clicked();
        void on_actionOpen_Cam_triggered();
        void on_updateTrajButton_clicked();
        void on_tabWidgetData_selected(QString );
        void on_updateFixButton_clicked();
        void on_doubleSpinBoxWinSize_valueChanged(double );
        void on_actionReport_triggered();
        void on_radioButtonAcceleration_toggled(bool checked);
        void on_groupBoxVectors_toggled(bool );
        void on_closedImgView();
        void PressPlay();
	void PressStop();
	void PressStepFoward();
	void PressStepBack();
	void SliderChange( int i);
	void CheckGrid(int checked);
	void TextGrid(const QString &text);
        void freeTrajPoints();
        void FreeFixPoints();
	void FileOpen();
	void ExportAngle();
	void ExportTrajectory();
	void StartCalibration();
	void oKDialogCalibration();
	void cancelDialogCalibration();
	void selectVelocity();
	void selectAcceleration();
	void selectTrabalho();
        void selectX_Trajectory();
        void selectY_Trajectory();
	void aboutClicked();

	// slots from model
	void calibrateOk(bool ok);
	void addCurve(int index, int r, int g, int b);
        void updateVelocity(int index, QVector <double> time, QVector <double> velocity, int actualTime );
        void updateAcceleration(int index, QVector <double> time, QVector <double> acceleration, int actualTime );
        void updateWork(int index, QVector <double> time, QVector <double> work, int actualTime );
        void updateTraj_x(int index, QVector <double> time, QVector <double> traj_x, int actualTime );
        void updateTraj_y(int index, QVector <double> time, QVector <double> traj_y, int actualTime );
        void updateKeyboard(int c);
        void updateImage(Mat image);
        void newTrajPoint();
        void newAnglePoint();

protected:
        void closeEvent(QCloseEvent *event);
        void keyPressEvent(QKeyEvent *e);

};

}

#endif /* CVMOBMAINWINDOW_H_ */
