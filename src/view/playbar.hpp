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
    enum PlayState {
        NoVideo = 0,
        Playing,
        Paused
    };

    explicit PlayBar(QWidget *parent = 0);
    ~PlayBar();

public slots:
    void setPlayData(int frames, int frameDuration);
    void setPlayState(PlayState state);

protected:
    void timerEvent(QTimerEvent *);

signals:
    void frameChanged(int frame);
    
private:
    PlayState _playState;
    int _frameDuration;
    int _currentTimer;
    Ui::PlayBar *_ui;

private slots:
    void playPause();
    void checkCurrentFrame(int frame);
};

#endif // PLAYERBAR_HPP
