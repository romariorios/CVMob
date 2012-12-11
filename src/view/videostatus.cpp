#include "videostatus.hpp"
#include "ui_videostatus.h"

#include <model/basejob.hpp>

#include <QTimer>

VideoStatus::VideoStatus(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::VideoStatus)
{
    _ui->setupUi(this);
    _ui->progressBar->hide();
}

VideoStatus::~VideoStatus()
{
    delete _ui;
}

void VideoStatus::setMessage(const QString &message)
{
    _ui->message->setText(message);
}

void VideoStatus::setJob(const QString &message, BaseJob *job)
{
    show();
    _ui->message->setText(message);
    _ui->progressBar->show();
    connect(job, SIGNAL(progressRangeChanged(int,int)),
            _ui->progressBar, SLOT(setRange(int,int)));
    connect(job, SIGNAL(progressChanged(int)),
            _ui->progressBar, SLOT(setValue(int)));
    connect(job, SIGNAL(finished()),
            _ui->progressBar, SLOT(hide()));
    connect(job, SIGNAL(finished()),
            SLOT(setFinishedMessage()));
}

void VideoStatus::setFinishedMessage()
{
    _ui->message->setText(tr("Done"));
    QTimer::singleShot(5000, this, SLOT(hide()));
}
