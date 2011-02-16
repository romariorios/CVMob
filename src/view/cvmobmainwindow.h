#ifndef CVMOBMAINWINDOW_H
#define CVMOBMAINWINDOW_H

#include <QMainWindow>

#include <opencv/cv.h>
#include <QList>

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
    QStandardItemModel *_tableModelFixPoints;
    QStandardItemModel *_tableModelAnglePoints;
    QStandardItemModel *_tableModelTrajPoints;
    imageViewer *_imageViewer;
    int _totalFrames;

	void initializePlots();
	void atualizeTableFixPoints();
	void atualizeTableAnglePoints();
	void atualizeTableTrajPoints();
	double calcDistance(CvPoint2D32f p1,CvPoint2D32f p2,float hR,float vR);
	void MakesModelConnections();

protected slots:
	void openFile();

public:
    explicit CvMobMainWindow(QWidget *parent = 0);
    ~CvMobMainWindow();
};

#endif // CVMOBMAINWINDOW_H
