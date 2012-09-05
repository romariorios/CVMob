/*
    CVMob - Motion capture program
    Copyright (C) 2012  The CVMob contributors

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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

protected:
    void timerEvent(QTimerEvent *);

signals:
    void frameChanged(int frame);
    
private:
    int _frameCount;
    int _frameDuration;
    int _currentTimer;
    Ui::PlayBar *_ui;

private slots:
    void checkCurrentFrame(int frame);
};

#endif // PLAYERBAR_HPP
