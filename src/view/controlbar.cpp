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

#include "controlbar.hpp"

#include <model/jobs/jobhandler.hpp>
#include <QMenu>

#include <algorithm>
#include <initializer_list>
#include <QPushButton>
#include <QSettings>
#include <QTimer>

using namespace std;

long ControlBar::Message::newId = 0;

ControlBar::ControlBar(QWidget *parent) :
    QWidget(parent),
    _frameCount(0),
    _frameDuration(0),
    _videoFrameDuration(0),
    _currentTimer(0)
{
    _ui.setupUi(this);
    _ui.playPauseButton->setDefaultAction(_ui.actionPlay);
    _ui.previousFrameButton->setDefaultAction(_ui.actionPrevious_frame);
    _ui.nextFrameButton->setDefaultAction(_ui.actionNext_Frame);
    _ui.progressSlide->setTracking(false);
    _ui.statusWidget->hide();

    _ui.actionPlay->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    _ui.actionPrevious_frame->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_MediaSeekBackward));
    _ui.actionNext_Frame->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_MediaSeekForward));
    _ui.actionSettings->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    _ui.actionClose_status->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));

    QMenu *drawMenu = new QMenu(_ui.drawButton);
    drawMenu->addAction(_ui.actionMeasure_distance);
    drawMenu->addAction(_ui.actionCalculate_trajectory);
    drawMenu->addAction(_ui.actionTrack_angle);
    _ui.drawButton->setMenu(drawMenu);
    _ui.drawButton->setDefaultAction(_ui.actionMeasure_distance);

    QMenu *calibrateMenu = new QMenu { _ui.calibrateButton };
    calibrateMenu->addAction(_ui.actionCalibrate_scale);
    calibrateMenu->addAction(_ui.actionCalibrate_framerate);
    calibrateMenu->addAction(_ui.actionSet_origin_point);
    _ui.calibrateButton->setMenu(calibrateMenu);
    _ui.calibrateButton->setDefaultAction(_ui.actionCalibrate_scale);

    _ui.backgroundActivityButton->setDefaultAction(_ui.actionStop_background_activity);
    _ui.backgroundActivityButton->hide();

    _ui.settingsButton->setDefaultAction(_ui.actionSettings);

    _ui.closeStatusButton->setDefaultAction(_ui.actionClose_status);

    connect(_ui.progressSlide, SIGNAL(valueChanged(int)), SIGNAL(frameChanged(int)));
    connect(_ui.frameSpinBox, SIGNAL(valueChanged(int)), SIGNAL(frameChanged(int)));
    connect(this, SIGNAL(frameChanged(int)), _ui.progressSlide, SLOT(setValue(int)));
    connect(this, SIGNAL(frameChanged(int)), _ui.frameSpinBox, SLOT(setValue(int)));
    connect(_ui.actionPlay, SIGNAL(toggled(bool)), SLOT(setPlaying(bool)));
    connect(_ui.actionMeasure_distance, SIGNAL(triggered()), SIGNAL(newDistanceRequested()));
    connect(_ui.actionCalculate_trajectory, SIGNAL(triggered()), SIGNAL(newTrajectoryRequested()));
    connect(_ui.actionTrack_angle, SIGNAL(triggered(bool)), SIGNAL(newAngleRequested()));
    connect(_ui.actionCalibrate_scale, SIGNAL(triggered(bool)), SIGNAL(scaleCalibrationRequested()));
    connect(_ui.actionCalibrate_framerate, SIGNAL(triggered(bool)),
            SIGNAL(framerateCalibrationRequested()));
    connect(
        _ui.actionSet_origin_point,
        SIGNAL(triggered(bool)),
        SIGNAL(originPointSettingRequested()));
    connect(_ui.actionSettings, SIGNAL(triggered(bool)), SIGNAL(settingsRequested()));

    connect(_ui.progressSlide, &QSlider::valueChanged, [&](int frame)
    {
        if (!(frame < _frameCount) && _playing) {
            setPlaying(false);
            _ui.progressSlide->setValue(0);
        }
    });

    connect(_ui.progressSlide, &QSlider::sliderPressed, [&](){
        killTimer(_currentTimer);
    });

    connect(_ui.progressSlide, &QSlider::sliderReleased, [&](){
        setPlaying(_playing);
    });

    connect(_ui.actionClose_status, &QAction::triggered, [=]()
    {
        removeMessage(_statusQueue.begin());
        hideStatus();
    });

    connect(_ui.actionNext_Frame, &QAction::triggered, [this]()
    {
        auto newValue = _ui.frameSpinBox->value() + 10;
        _ui.frameSpinBox->setValue(newValue < _frameCount? newValue : _frameCount - 1);
    });

    connect(_ui.actionPrevious_frame, &QAction::triggered, [this]()
    {
        auto newValue = _ui.frameSpinBox->value() - 10;
        _ui.frameSpinBox->setValue(newValue >= 0? newValue : 0);
    });

    setPlayData(0, 0);
}

void ControlBar::setPlayData(int frameCount, int frameDuration)
{
    _ui.progressSlide->setMaximum(frameCount);
    _ui.progressSlide->setValue(0);

    for (auto w : initializer_list<QWidget *> {
        _ui.progressSlide,
        _ui.drawButton,
        _ui.calibrateButton,
        _ui.previousFrameButton,
        _ui.nextFrameButton
    }) {
        w->setEnabled(frameCount);
    }

    _ui.frameSpinBox->setMaximum(frameCount);
    _ui.frameSpinBox->setValue(0);
    _ui.frameSpinBox->setEnabled(frameCount);

    QSettings set;

    _videoFrameDuration = frameDuration;
    _frameDuration = set.value("video/respectFramerate", false).toBool()? frameDuration : 0;
    _frameCount = frameCount;
    setPlaying(false);

    _ui.playPauseButton->setEnabled(frameCount);

    emit playDataChanged(frameCount, frameDuration);
}

void ControlBar::setJobHandler(const shared_ptr<JobHandler> &jh)
{
    if (_jobHandler) {
        _jobHandler->disconnect();
    }

    _jobHandler = jh;

    if (!_jobHandler) {
        return;
    }

    connect(_ui.actionStop_background_activity, &QAction::triggered,
            _jobHandler.get(), &JobHandler::stopAll);
    connect(_jobHandler.get(), &QThread::finished,
            _ui.backgroundActivityButton, &QWidget::hide);

    connect(_jobHandler.get(), &JobHandler::jobAmountChanged,     // if amount == 0, then setVisible(0) gets
            _ui.backgroundActivityButton, &QWidget::setVisible); // called, which equals setVisible(false)
}

long ControlBar::enqueueMessage(const QString& message, int duration)
{
    _statusQueue << message;

    if (_statusQueue.size() == 1) {
        setStatusVisible();
        _ui.message->setText(message);
    }

    auto messageId = _statusQueue.last().id;

    if (duration != 0) {
        auto timer = new QTimer { this };
        timer->setSingleShot(true);
        timer->start(duration);

        connect(timer, &QTimer::timeout, [=]()
        {
            dequeueMessageWithId(messageId);
        });
    } else { // Persistent
        while (_statusQueue[0].id != messageId) {
            dequeueFirstMessage();
        }
    }

    return messageId;
}

void ControlBar::dequeueFirstMessage()
{
    removeMessage(_statusQueue.begin());
}

void ControlBar::updateSettings()
{
    QSettings set;

    _frameDuration = set.value("video/respectFramerate", false).toBool()? _videoFrameDuration : 0;
    if (_playing) {
        killTimer(_currentTimer);
        _currentTimer = startTimer(_frameDuration);
    }
}

void ControlBar::dequeueMessageWithId(long messageId)
{
    QList<Message>::iterator it = std::find_if(
        _statusQueue.begin(),
        _statusQueue.end(),
        [=](Message m) -> bool { return m.id == messageId; });

    removeMessage(it);
}

void ControlBar::removeMessage(const QList<ControlBar::Message>::iterator &it)
{
    if (it == _statusQueue.end()) {
        return;
    }

    _statusQueue.erase(it);

    if (_statusQueue.isEmpty()) {
        hideStatus();
    } else {
        _ui.message->setText(_statusQueue[0]);
    }
}

void ControlBar::timerEvent(QTimerEvent *e)
{
    _ui.progressSlide->setValue(_ui.progressSlide->value() + 1);
}

void ControlBar::setStatusVisible(bool visible)
{
    _ui.statusWidget->setVisible(visible);
    _ui.playWidget->setVisible(!visible);
}

void ControlBar::setPlaying(bool playing)
{
    _playing = playing;

    if (playing) {
        _currentTimer = startTimer(_frameDuration);
    } else {
        killTimer(_currentTimer);
    }

    _ui.actionPlay->setText(playing? tr("Pause") : tr("Play"));
    _ui.actionPlay->setToolTip(playing? tr("Pause playback") : tr("Start playback"));
    _ui.actionPlay->setIcon(playing? QApplication::style()->standardIcon(QStyle::SP_MediaPause) :
                                      QApplication::style()->standardIcon(QStyle::SP_MediaPlay));

    disconnect(_ui.actionPlay, SIGNAL(toggled(bool)), this, SLOT(setPlaying(bool)));
    _ui.actionPlay->setChecked(playing);
    connect(_ui.actionPlay, SIGNAL(toggled(bool)), SLOT(setPlaying(bool)));

    emit playingChanged(playing);
}
