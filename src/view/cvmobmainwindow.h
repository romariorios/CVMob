#ifndef CVMOBMAINWINDOW_H
#define CVMOBMAINWINDOW_H

#include <QMainWindow>

#include <opencv/cv.h>
#include <QList>

class CvmobModel;
class QStandardItemModel;
class VideoView;

namespace Ui {
    class CvMobMainWindow;
}

class CvMobMainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::CvMobMainWindow *_ui;
    CvmobModel *_distancesModel;
    VideoView *_videoView;

private slots:

protected slots:
	void openFile();

public:
    explicit CvMobMainWindow(QWidget *parent = 0);
    ~CvMobMainWindow();
};

#endif // CVMOBMAINWINDOW_H
