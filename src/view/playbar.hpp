/*
    CVMob - Motion capture program
    Copyright (C) 2013, 2014  The CVMob contributors

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

#ifndef PLAYERBAR_HPP
#define PLAYERBAR_HPP

#include <QWidget>

namespace Ui {
class PlayBar;
}

class QTimerEvent;

class PlayBar : public QWidget
{
    Q_OBJECT

public:
    explicit PlayBar(QWidget *parent = 0);
    ~PlayBar();

public slots:
    void setPlaying(bool playing);
    void setPlayData(int frames, int frameDuration);
    void updateSettings();

protected:
    void timerEvent(QTimerEvent *);

signals:
    void playingChanged(bool playing);
    void playDataChanged(int frames, int frameDuration);

    void frameChanged(int frame);
    void newAngleRequested();
    void newDistanceRequested();
    void newTrajectoryRequested();
    void scaleCalibrationRequested();
    void settingsRequested();

private:
    int _frameCount;
    int _frameDuration;
    int _videoFrameDuration;
    int _currentTimer;
    bool _playing;
    Ui::PlayBar *_ui;
    bool _playStatus;
};

#endif // PLAYERBAR_HPP
