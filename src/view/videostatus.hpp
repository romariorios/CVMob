#ifndef VIDEOSTATUS_HPP
#define VIDEOSTATUS_HPP

#include <QWidget>

namespace Ui {
class VideoStatus;
}

class VideoStatus : public QWidget
{
    Q_OBJECT
    
public:
    explicit VideoStatus(QWidget *parent = 0);
    ~VideoStatus();

    void setMessage(const QString &message);
    
private:
    Ui::VideoStatus *_ui;
};

#endif // VIDEOSTATUS_HPP
