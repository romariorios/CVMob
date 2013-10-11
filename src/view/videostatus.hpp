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

#ifndef VIDEOSTATUS_HPP
#define VIDEOSTATUS_HPP

#include <QWidget>

#include <QList>

namespace Status {
class Base;
class JobHandler;
}

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
    
    void addJob(BaseJob *j);

    friend class Status::Base;
    
private slots:
    void callItDone();
    
private:
    Ui::VideoStatus *_ui;
    QList<Status::Base *> _statusQueue;
    Status::JobHandler *_jobHandler;
};

namespace Status
{

class Base : public QObject
{
public:
    ~Base();

protected:
    VideoStatus *_parent;

    Base(VideoStatus *parent, QString message);
    
    inline QList<Status::Base *> &statusQueue() const { return _parent->_statusQueue; }

private:
    QString _message;
};

class Message : public Base
{
public:
    Message(VideoStatus *parent, QString message, int period = 5000);
};

class Persistent : public Base
{
public:
    Persistent(VideoStatus *parent, QString message);
    ~Persistent();
};

class JobHandler : public QObject
{
    Q_OBJECT
public:
    explicit JobHandler(VideoStatus* parent);
    
    void addJob(BaseJob* j);
    inline int jobAmount() const { return _progress.size(); }
    
signals:
    void rangeChanged(int minimum, int maximum);
    void progressChanged(int progress);
    void allFinished();
    void jobAmountChanged(int amount);
    
private:
    void onJobRangeChanged(BaseJob *j, int maximum);
    void onJobProgressChanged(BaseJob *j, int progress);
    void onJobFinished(BaseJob *j);
    
    struct Progress {
        Progress(int m = 0, int v = 0) : maximum(m), value(v) {}
        int maximum;
        int value;
    };
    QHash<BaseJob *, Progress> _progress;
    int _maximum;
    int _curProgress;
};

}

#endif // VIDEOSTATUS_HPP
