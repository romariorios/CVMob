/*
    CVMob - Motion capture program
    Copyright (C) 2012  The CVMob contributors

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

void VideoStatus::setMessage(const QString &message)
{
    show();
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
