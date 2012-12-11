#ifndef VIDEOSTATUS_HPP
#define VIDEOSTATUS_HPP

#include <QWidget>

namespace Ui {
class VideoStatus;
}

class BaseJob;

class VideoStatus : public QWidget
{
    Q_OBJECT
    
public:
    explicit VideoStatus(QWidget *parent = 0);
    ~VideoStatus();

    void setMessage(const QString &message);
    void setJob(const QString &message, BaseJob *job);
    
private:
    Ui::VideoStatus *_ui;

private slots:
    void setFinishedMessage();
};

#endif // VIDEOSTATUS_HPP
