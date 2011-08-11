#ifndef CVMOBMAINWINDOW_H
#define CVMOBMAINWINDOW_H

#include <QMainWindow>

#include <opencv/cv.h>
#include <QList>

class CvmobVideoModel;
class QStandardItemModel;
class VideoView;

namespace Ui {
    class CvMobMainWindow;
}

class CvMobMainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::CvMobMainWindow *_ui;
    CvmobVideoModel *_distancesModel;
    VideoView *_videoView;

private slots:

protected slots:
	void openFile();

public:
    explicit CvMobMainWindow(QWidget *parent = 0);
    ~CvMobMainWindow();
};

#endif // CVMOBMAINWINDOW_H
