/*
    CVMob - Motion capture program
    Copyright (C) 2013--2015  The CVMob contributors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CVMOBMAINWINDOW_H
#define CVMOBMAINWINDOW_H

#include <QMainWindow>

#include <opencv/cv.h>
#include <QList>

#include <model/videomodel.hpp>
#include <model/proxies/anglesproxymodel.hpp>
#include <model/proxies/distancesproxymodel.hpp>
#include <model/proxies/plotproxies.hpp>
#include <model/proxies/trajectoriesproxymodel.hpp>
#include <model/proxies/videolistproxymodel.hpp>
#include <view/plotitemview.hpp>
#include <view/settings.hpp>
#include <view/videoview.hpp>

#include "ui_cvmobmainwindow.h"

class CvMobMainWindow : public QMainWindow
{
    Q_OBJECT

protected slots:
    void openFile();
    void setDockWasShown(bool shown);

public:
    explicit CvMobMainWindow(QWidget *parent = 0);

private:
    Ui::CvMobMainWindow _ui;

    VideoModel _videoModel;
    VideoListProxyModel _videoNamesModel;
    DistancesProxyModel _distancesModel;
    TrajectoriesProxyModel _trajectoriesModel;
    AnglesProxyModel _anglesModel;
    PlotItemView
        _xPlot,
        _yPlot,
        _speedPlot,
        _accelPlot,
        _anglePlot;
    XTrajectoryPlotProxyModel _xPlotModel;
    YTrajectoryPlotProxyModel _yPlotModel;
    TrajectorySpeedPlotProxyModel _speedPlotModel;
    TrajectoryAccelPlotProxyModel _accelPlotModel;
    AnglePlotProxyModel _anglePlotModel;

    VideoView _videoView;
    Settings _settingsWidget;
    short _closedDocks = 0;
};

#endif // CVMOBMAINWINDOW_H
