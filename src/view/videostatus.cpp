#include "videostatus.hpp"
#include "ui_videostatus.h"

VideoStatus::VideoStatus(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::VideoStatus)
{
    _ui->setupUi(this);
}

VideoStatus::~VideoStatus()
{
    delete _ui;
}

void VideoStatus::setMessage(const QString &message)
{
    _ui->message->setText(message);
}

