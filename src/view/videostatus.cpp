/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

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

#include "videostatus.hpp"
#include "ui_videostatus.h"

#include <model/jobs/basejob.hpp>

#include <QTimer>

VideoStatus::VideoStatus(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::VideoStatus)
{
    _jobHandler = new Status::JobHandler(this);
    _ui->setupUi(this);
    _ui->messageWidget->hide();
    _ui->progressWidget->hide();
    
    connect(_jobHandler, &Status::JobHandler::rangeChanged,
            _ui->progressBar, &QProgressBar::setRange);
    connect(_jobHandler, &Status::JobHandler::progressChanged,
            _ui->progressBar, &QProgressBar::setValue);
    connect(_jobHandler, &Status::JobHandler::allFinished,
            _ui->progressWidget, &QWidget::hide);
    connect(_jobHandler, &Status::JobHandler::allFinished,
            this, &VideoStatus::callItDone);
    
    connect(_jobHandler, &Status::JobHandler::jobAmountChanged,
            [this](int amount){
                _ui->progressText->setText(tr("Calculating (%n jobs remain)", "",
                                              amount));
            });
}

VideoStatus::~VideoStatus()
{
    delete _ui;
}

void VideoStatus::addJob(BaseJob *j)
{
    _jobHandler->addJob(j);
    _ui->progressWidget->show();
}

void VideoStatus::callItDone()
{
    new Status::Message(this, tr("Done"), 3000);
}

// StatusItems
Status::Base::Base(VideoStatus *parent, QString message) :
    _parent(parent),
    _message(message)
{
    _parent->_statusQueue << this;
    if (_parent->_statusQueue.size() == 1) {
        _parent->show();
        _parent->_ui->messageWidget->show();
        _parent->_ui->message->setText(_parent->_statusQueue[0]->_message);
    }
}

Status::Base::~Base()
{
    int index = _parent->_statusQueue.indexOf(this);
    _parent->_statusQueue.removeAt(index);

    if (_parent->_statusQueue.isEmpty()) {
        _parent->_ui->messageWidget->hide();
    } else {
        _parent->_ui->message->setText(_parent->_statusQueue[0]->_message);
    }
}

Status::Message::Message(VideoStatus *parent, QString message, int period) :
    Base(parent, message)
{
    QTimer::singleShot(period, this, SLOT(deleteLater()));
}

Status::Persistent::Persistent(VideoStatus *parent, QString message) :
    Base(parent, message)
{
    while (statusQueue()[0] != this) {
        delete statusQueue()[0];
    }
}

Status::Persistent::~Persistent()
{}

void Status::JobHandler::addJob(BaseJob* j)
{
    _progress[j] = Progress();
    connect(j, &BaseJob::progressRangeChanged, [this, j](int min, int max){
        onJobRangeChanged(j, max);
    });
    connect(j, &BaseJob::progressChanged, [this, j](int progress){
        onJobProgressChanged(j, progress);
    });
    connect(j, &QThread::finished, [this, j](){
        onJobFinished(j);
    });
    
    emit jobAmountChanged(jobAmount());
}

Status::JobHandler::JobHandler(VideoStatus* parent) :
    QObject(parent),
    _maximum(0),
    _curProgress(0)
{}

void Status::JobHandler::onJobRangeChanged(BaseJob* j, int maximum)
{
    int oldMaximum = _progress[j].maximum;
    _progress[j].maximum = maximum;
    
    _maximum += maximum - oldMaximum;
    emit rangeChanged(0, _maximum);
}

void Status::JobHandler::onJobProgressChanged(BaseJob* j, int progress)
{
    int oldProgress = _progress[j].value;
    _progress[j].value = progress;
    
    _curProgress += progress - oldProgress;
    emit progressChanged(_curProgress);
}

void Status::JobHandler::onJobFinished(BaseJob* j)
{
    _maximum -= _progress[j].maximum;
    _curProgress -= _progress[j].maximum;
    
    emit rangeChanged(0, _maximum);
    emit progressChanged(_curProgress);
    
    _progress.remove(j);
    if (_progress.empty()) {
        emit allFinished();
    }
    
    emit jobAmountChanged(jobAmount());
}
