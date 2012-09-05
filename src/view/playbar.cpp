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

#include "playbar.hpp"
#include "ui_playbar.h"

PlayBar::PlayBar(QWidget *parent) :
    QWidget(parent),
    _frameCount(0),
    _frameDuration(0),
    _currentTimer(0),
    _ui(new Ui::PlayBar)
{
    _ui->setupUi(this);
    _ui->playPauseButton->setDefaultAction(_ui->actionPlay);

    connect(_ui->progressSlide, SIGNAL(valueChanged(int)), SIGNAL(frameChanged(int)));
    connect(_ui->progressSlide, SIGNAL(valueChanged(int)), SLOT(checkCurrentFrame(int)));
    connect(_ui->actionPlay, SIGNAL(toggled(bool)), SLOT(setPlaying(bool)));

    setPlayData(0, 0);
}

PlayBar::~PlayBar()
{
    delete _ui;
}


void PlayBar::setPlayData(int frameCount, int frameDuration)
{
    _ui->progressSlide->setMaximum(frameCount);
    _ui->progressSlide->setValue(0);
    _ui->progressSlide->setEnabled(frameCount);
    _ui->actionPlay->setEnabled(frameCount);
    _frameDuration = frameDuration;
    _frameCount = frameCount;
    setPlaying(false);
}

void PlayBar::timerEvent(QTimerEvent *)
{
    _ui->progressSlide->setValue(_ui->progressSlide->value() + 1);
}

void PlayBar::setPlaying(bool playing)
{
    if (playing) {
        _currentTimer = startTimer(_frameDuration);
    } else {
        killTimer(_currentTimer);
    }
    
    _ui->actionPlay->setText(playing? tr("Pause") : tr("Play"));
    _ui->actionPlay->setToolTip(playing? tr("Pause playback") : tr("Start playback"));
    _ui->actionPlay->setIcon(QIcon(playing? ":/images/icons/media-playback-pause" :
                                            ":/images/icons/media-playback-start"));
    
    disconnect(_ui->actionPlay, SIGNAL(toggled(bool)), this, SLOT(setPlaying(bool)));
    _ui->actionPlay->setChecked(playing);
    connect(_ui->actionPlay, SIGNAL(toggled(bool)), SLOT(setPlaying(bool)));
}

void PlayBar::checkCurrentFrame(int frame)
{
    if (!(frame < _frameCount)) {
        setPlaying(false);
        _ui->progressSlide->setValue(0);
    }
}
