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
    _playState(NoVideo),
    _frameDuration(0),
    _currentTimer(0),
    _ui(new Ui::PlayBar)
{
    _ui->setupUi(this);
    _ui->playPauseButton->setDefaultAction(_ui->actionPlay_Pause);

    connect(_ui->progressSlide, SIGNAL(valueChanged(int)), SIGNAL(frameChanged(int)));
    connect(_ui->progressSlide, SIGNAL(valueChanged(int)), SLOT(checkCurrentFrame(int)));
    connect(_ui->actionPlay_Pause, SIGNAL(triggered()), SLOT(playPause()));

    setPlayData(0, 0);
}

PlayBar::~PlayBar()
{
    delete _ui;
}


void PlayBar::setPlayData(int frames, int frameDuration)
{
    _ui->progressSlide->setMaximum(frames);
    _ui->progressSlide->setValue(0);
    _ui->progressSlide->setEnabled(frames);
    _ui->actionPlay_Pause->setEnabled(frames);
    _frameDuration = frameDuration;
    setPlayState(frames? Paused : NoVideo);
}

void PlayBar::setPlayState(PlayBar::PlayState state)
{
    if (state == Paused || NoVideo) {
        killTimer(_currentTimer);
    } else if (state == Playing) {
        _currentTimer = startTimer(_frameDuration);
    }

    _playState = state;
}

void PlayBar::timerEvent(QTimerEvent *)
{
    _ui->progressSlide->setValue(_ui->progressSlide->value() + 1);
}

void PlayBar::playPause()
{
    switch (_playState) {
    case Paused:
        setPlayState(Playing);
        break;
    case Playing:
        setPlayState(Paused);
        break;
    default:
        break;
    }
}

void PlayBar::checkCurrentFrame(int frame)
{
    if (frame >= _ui->progressSlide->maximum() - 1) {
        setPlayState(Paused);
        _ui->progressSlide->setValue(0);
    }
}
