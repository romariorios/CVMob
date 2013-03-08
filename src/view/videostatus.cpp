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

void VideoStatus::clearQueue()
{
    while (!_statusQueue.isEmpty()) {
        delete _statusQueue.at(0);
    }
}

// StatusItems
Status::Base::Base(VideoStatus *parent, QString message) :
    _parent(parent),
    _message(message)
{
    _parent->_statusQueue << this;
    if (_parent->_statusQueue.size() == 1) {
        _parent->show();
        _parent->_ui->message->setText(_parent->_statusQueue[0]->_message);
    }
}

Status::Base::~Base()
{
    int index = _parent->_statusQueue.indexOf(this);
    _parent->_statusQueue.removeAt(index);

    if (_parent->_statusQueue.isEmpty()) {
        _parent->hide();
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
{}

Status::Persistent::~Persistent()
{}

Status::Job::Job(VideoStatus *parent, QString message, BaseJob *job) :
    Base(parent, message)
{
    parent->_ui->progressBar->show();
    connect(job, SIGNAL(progressRangeChanged(int,int)),
            parent->_ui->progressBar, SLOT(setRange(int,int)));
    connect(job, SIGNAL(progressChanged(int)),
            parent->_ui->progressBar, SLOT(setValue(int)));

    connect(job, SIGNAL(finished()), SLOT(callItDone()));
    connect(job, SIGNAL(finished()), SLOT(deleteLater()));
    connect(job, SIGNAL(finished()), parent->_ui->progressBar, SLOT(hide()));
}

void Status::Job::callItDone()
{
    new Status::Message(_parent, tr("Done"), 3000);
}
