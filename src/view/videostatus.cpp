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

#include "videostatus.hpp"
#include "ui_videostatus.h"

#include <model/jobs/basejob.hpp>
#include <model/jobs/jobhandler.hpp>

#include <QTimer>

VideoStatus::VideoStatus(QWidget* parent) :
    QWidget(parent),
    _ui(new Ui::VideoStatus),
    _jobHandler(0)
{
    _ui->setupUi(this);
    _ui->messageWidget->hide();
    _ui->progressWidget->hide();
}

VideoStatus::~VideoStatus()
{
    delete _ui;
}

void VideoStatus::setJobHandler(JobHandler* jh)
{
    if (_jobHandler) {
        _jobHandler->disconnect();
    }

    _jobHandler = jh;

    if (!_jobHandler) {
        return;
    }

    connect(_jobHandler, &JobHandler::rangeChanged,
            _ui->progressBar, &QProgressBar::setRange);
    connect(_jobHandler, &JobHandler::progressChanged,
            _ui->progressBar, &QProgressBar::setValue);
    connect(_jobHandler, &QThread::finished,
            _ui->progressWidget, &QWidget::hide);
    connect(_jobHandler, &QThread::finished,
            this, &VideoStatus::callItDone);
    connect(_jobHandler, &QObject::destroyed, [&](){
        _jobHandler = 0;
    });

    connect(_jobHandler, &JobHandler::jobAmountChanged,
            [this](int amount){
                _ui->progressText->setText(tr("Calculating (%n jobs remain)", "",
                                              amount));
            });
    connect(_jobHandler, &JobHandler::jobAmountChanged, // if amount == 0, then setVisible(0) gets
            _ui->progressWidget, &QWidget::setVisible); // called, which equals setVisible(false)
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
