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

#ifndef CONTROLBAR_HPP
#define CONTROLBAR_HPP

#include <QWidget>

namespace Ui {
class ControlBar;
}

class BaseJob;
class JobHandler;
class QTimerEvent;

class ControlBar : public QWidget
{
    Q_OBJECT

public:
    explicit ControlBar(QWidget *parent = nullptr);
    ~ControlBar();

public slots:
    void setPlaying(bool playing);
    void setPlayData(int frames, int frameDuration);
    void setJobHandler(JobHandler *jh);
    long enqueueMessage(const QString &message, int duration = 5000);
    void dequeueFirstMessage();
    void dequeueMessageWithId(long messageId);
    inline void pushPersistentMessage(const QString &message) { enqueueMessage(message, 0); }
    void updateSettings();

protected:
    void timerEvent(QTimerEvent *e);

signals:
    void playingChanged(bool playing);
    void playDataChanged(int frames, int frameDuration);

    void frameChanged(int frame);
    void newAngleRequested();
    void newDistanceRequested();
    void newTrajectoryRequested();
    void scaleCalibrationRequested();
    void framerateCalibrationRequested();
    void settingsRequested();

private:
    struct Message
    {
        Message(const QString &message) :
            message { message }
        {}

        operator QString() { return message; }

        long id = newId++;
        QString message;

        static long newId;
    };

    void removeMessage(const QList<Message>::iterator &it);

    int _frameCount;
    int _frameDuration;
    int _videoFrameDuration;
    int _currentTimer;
    bool _playing;
    Ui::ControlBar *_ui;
    bool _playStatus;
    JobHandler *_jobHandler = nullptr;
    QList<Message> _statusQueue;

private slots:
    void setStatusVisible(bool visible = true);
    inline void hideStatus() { setStatusVisible(false); }
};

#endif // CONTROLBAR_HPP
