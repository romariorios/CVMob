/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

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

#include "playbar.hpp"
#include "ui_playbar.h"

#include <QMenu>

#include <QPushButton>
#include <QSettings>

PlayBar::PlayBar(QWidget *parent) :
    QWidget(parent),
    _frameCount(0),
    _frameDuration(0),
    _videoFrameDuration(0),
    _currentTimer(0),
    _ui(new Ui::PlayBar)
{
    _ui->setupUi(this);
    _ui->playPauseButton->setDefaultAction(_ui->actionPlay);
    _ui->progressSlide->setTracking(false);

    _ui->actionPlay->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    _ui->actionSettings->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));

    QMenu *drawMenu = new QMenu(_ui->drawButton);
    drawMenu->addAction(_ui->actionMeasure_distance);
    drawMenu->addAction(_ui->actionCalculate_trajectory);
    drawMenu->addAction(_ui->actionTrack_angle);
    _ui->drawButton->setMenu(drawMenu);
    _ui->drawButton->setDefaultAction(_ui->actionMeasure_distance);

    _ui->settingsButton->setDefaultAction(_ui->actionSettings);

    connect(_ui->progressSlide, SIGNAL(valueChanged(int)), SIGNAL(frameChanged(int)));
    connect(_ui->actionPlay, SIGNAL(toggled(bool)), SLOT(setPlaying(bool)));
    connect(_ui->actionMeasure_distance, SIGNAL(triggered()), SIGNAL(newDistanceRequested()));
    connect(_ui->actionCalculate_trajectory, SIGNAL(triggered()), SIGNAL(newTrajectoryRequested()));
    connect(_ui->actionTrack_angle, SIGNAL(triggered(bool)), SIGNAL(newAngleRequested()));
    connect(_ui->actionSettings, SIGNAL(triggered(bool)), SIGNAL(settingsRequested()));

    connect(_ui->progressSlide, &QSlider::valueChanged, [&](int frame)
    {
        if (!(frame < _frameCount) && _playing) {
            setPlaying(false);
            _ui->progressSlide->setValue(0);
        }
    });

    connect(_ui->progressSlide, &QSlider::sliderPressed, [&](){
        killTimer(_currentTimer);
    });

    connect(_ui->progressSlide, &QSlider::sliderReleased, [&](){
        setPlaying(_playing);
    });

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
    _ui->drawButton->setEnabled(frameCount);

    QSettings set;

    _videoFrameDuration = frameDuration;
    _frameDuration = set.value("video/respectFramerate", false).toBool()? frameDuration : 0;
    _frameCount = frameCount;
    setPlaying(false);

    _ui->playPauseButton->setEnabled(frameCount);

    emit playDataChanged(frameCount, frameDuration);
}

void PlayBar::updateSettings()
{
    QSettings set;

    _frameDuration = set.value("video/respectFramerate", false).toBool()? _videoFrameDuration : 0;
    if (_playing) {
        killTimer(_currentTimer);
        _currentTimer = startTimer(_frameDuration);
    }
}

void PlayBar::timerEvent(QTimerEvent *)
{
    _ui->progressSlide->setValue(_ui->progressSlide->value() + 1);
}

void PlayBar::setPlaying(bool playing)
{
    _playing = playing;

    if (playing) {
        _currentTimer = startTimer(_frameDuration);
    } else {
        killTimer(_currentTimer);
    }

    _ui->actionPlay->setText(playing? tr("Pause") : tr("Play"));
    _ui->actionPlay->setToolTip(playing? tr("Pause playback") : tr("Start playback"));
    _ui->actionPlay->setIcon(playing? QApplication::style()->standardIcon(QStyle::SP_MediaPause) :
                                      QApplication::style()->standardIcon(QStyle::SP_MediaPlay));

    disconnect(_ui->actionPlay, SIGNAL(toggled(bool)), this, SLOT(setPlaying(bool)));
    _ui->actionPlay->setChecked(playing);
    connect(_ui->actionPlay, SIGNAL(toggled(bool)), SLOT(setPlaying(bool)));

    emit playingChanged(playing);
}
