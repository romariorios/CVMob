#ifndef CVMOBMAINWINDOW_H
#define CVMOBMAINWINDOW_H

#include <QMainWindow>

#include <opencv/cv.h>
#include <QList>

class FixedPointsDistancesListModel;
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
    FixedPointsDistancesListModel *_fixedPointsDistancesModel;
    QStandardItemModel *_tableModelAnglePoints;
    QStandardItemModel *_tableModelTrajPoints;
    imageViewer *_imageViewer;
    int _totalFrames;

	void initializePlots();
	void atualizeTableAnglePoints();
	void atualizeTableTrajPoints();
	void MakesModelConnections();

protected slots:
	void openFile();

public:
    explicit CvMobMainWindow(QWidget *parent = 0);
    ~CvMobMainWindow();
};

#endif // CVMOBMAINWINDOW_H
