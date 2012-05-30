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
    CvmobVideoModel *_videoModel;
    VideoView *_videoView;

private slots:

protected slots:
	void openFile();

public:
    explicit CvMobMainWindow(QWidget *parent = 0);
    ~CvMobMainWindow();

public slots:
    void showInternalData() const;
};

#endif // CVMOBMAINWINDOW_H
