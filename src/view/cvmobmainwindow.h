#ifndef CVMOBMAINWINDOW_H
#define CVMOBMAINWINDOW_H

#include <QMainWindow>
#include <QList>

class Plot;
namespace Ui {
    class CvMobMainWindow;
}

class CvMobMainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::CvMobMainWindow *_ui;
    QList<Plot *> _plots;

    void initializePlots();

public:
    explicit CvMobMainWindow(QWidget *parent = 0);
    ~CvMobMainWindow();
};

#endif // CVMOBMAINWINDOW_H
