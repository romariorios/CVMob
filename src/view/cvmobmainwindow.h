#ifndef CVMOBMAINWINDOW_H
#define CVMOBMAINWINDOW_H

#include <QMainWindow>

#include <opencv/cv.h>
#include <QList>

#include "view/imageviewer.h"

class FixedPointsTableModel;
class imageViewer;
class Plot;
class QStandardItemModel;

namespace Ui {
    class CvMobMainWindow;
}

class CvMobMainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::CvMobMainWindow *_ui;
    QList<Plot *> _plots;
    FixedPointsTableModel *_fixedPointsDistancesModel;
    QStandardItemModel *_tableModelAnglePoints;
    QStandardItemModel *_tableModelTrajPoints;
    imageViewer *_imageViewer;
    int _totalFrames;

	void initializePlots();
	void atualizeTableAnglePoints();
	void atualizeTableTrajPoints();
	void MakesModelConnections();

private slots:
	// Deprecated (but still needed) slots

	void calibrateOk(bool ok);
	void addCurve(int index, int r, int g, int b);
	void updateVelocity(int index, QVector <double> time, QVector <double> velocity, int actualTime );
	void updateAcceleration(int index, QVector <double> time, QVector <double> acceleration, int actualTime );
	void updateWork(int index, QVector <double> time, QVector <double> work, int actualTime );
	void updateTraj_x(int index, QVector <double> time, QVector <double> traj_x, int actualTime );
	void updateTraj_y(int index, QVector <double> time, QVector <double> traj_y, int actualTime );
	void updateImage(Mat image);
	void newTrajPoint();
	void newAnglePoint();

	// Deprecated end

protected slots:
	void openFile();

public:
    explicit CvMobMainWindow(QWidget *parent = 0);
    ~CvMobMainWindow();

    // Deprecated (but still needed methods)

    void calibratePlots();
    void freeTrajPoints();

    // Deprecated end
};

#endif // CVMOBMAINWINDOW_H
