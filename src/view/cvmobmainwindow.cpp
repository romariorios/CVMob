#include "cvmobmainwindow.h"
#include "ui_cvmobmainwindow.h"

#include "graphs/Plot.h"

CvMobMainWindow::CvMobMainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CvMobMainWindow)
{
    _ui->setupUi(this);

    _ui->playPauseButton->setDefaultAction(_ui->actionPlay_Pause);
    _ui->openButton->setDefaultAction(_ui->action_Open);

    initializePlots();
}

void CvMobMainWindow::initializePlots()
{
	_plots << new Plot(tr("Velocity"), tr("Frames") , tr("vel (pxl/frame)"))
		   << new Plot(tr("Acceleration"), tr("Frames"), tr("acc (pxl/frame²)"))
		   << new Plot(tr("x(t)"), tr("Time"), tr("x(t) (pxl)"))
		   << new Plot(tr("y(t)"), tr("time"), tr("y(t) (pxl)"));

	foreach (Plot *plot, _plots) {
		plot->setAutoReplot(true);
		plot->hide();
		_ui->graphsLayout->insertWidget(0, plot);
	}

	_plots.at(0)->show();
	_plots.at(1)->show();
}

CvMobMainWindow::~CvMobMainWindow()
{
	delete _ui;
}
