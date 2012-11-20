#include "lineartrajectorycalcjob.hpp"

#include <model/videomodel.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include <QSize>

#include <QDebug>

using namespace cv;
using namespace std;

LinearTrajectoryCalcJob::LinearTrajectoryCalcJob(const QPointF &startPoint,
                                                 int videoRow,
                                                 const QSize &windowSize,
                                                 QAbstractItemModel *parent) :
    QThread(parent),
    _startPoint(startPoint),
    _videoRow(videoRow),
    _windowSize(windowSize),
    _model(parent)
{
    connect(this, SIGNAL(instantGenerated(int,QPointF,QPointF,QPointF)),
            &_target, SLOT(storeInstant(int,QPointF,QPointF,QPointF)), Qt::QueuedConnection);
    _target.model = _model;
}

void LinearTrajectoryCalcJob::setTarget(const QModelIndex &targetIndex)
{
    if (isRunning() || isFinished()) {
        return;
    }

    _target.parentIndex = targetIndex;
}

void LinearTrajectoryCalcJob::run()
{
    QModelIndex framesParentIndex = _model->index(_videoRow, VideoModel::FramesColumn);

    emit instantGenerated(0, _startPoint, QPointF(0, 0), QPointF(0, 0));
    emit rangeChanged(0, _model->rowCount(framesParentIndex) - 1);
    emit progressChanged(0);

    for (int i = 1; i < _model->rowCount(framesParentIndex); ++i) {
        QImage startFrame(_model->index(i - 1, 0, framesParentIndex).data(VideoModel::VideoSceneRole)
                          .value<QImage>().convertToFormat(QImage::Format_Indexed8));
        QImage endFrame(_model->index(i, 0, framesParentIndex).data(VideoModel::VideoSceneRole)
                        .value<QImage>().convertToFormat(QImage::Format_Indexed8));
        Mat cvStartFrame(startFrame.height(), startFrame.width(), CV_8UC1, startFrame.scanLine(0));
        Mat cvEndFrame(endFrame.height(), endFrame.width(), CV_8UC1, endFrame.scanLine(0));
        Point2f cvStartPoint(_startPoint.x(), _startPoint.y());
        vector<Point2f> startPoints;
        vector<Point2f> endPoints;
        vector<uchar> status;
        vector<float> err;
        Size cvWindowSize;

        cvWindowSize.height = _windowSize.height();
        cvWindowSize.width = _windowSize.width();
        startPoints.push_back(cvStartPoint);

        calcOpticalFlowPyrLK(cvStartFrame, cvEndFrame, startPoints, endPoints, status, err,
                             cvWindowSize,
                             // Magic numbers:
                             3, TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 20, 0.01),
                             0.5, 0);

        QPointF newPoint(endPoints[0].x, endPoints[0].y);
        _target.mutex.lock();
            const QModelIndex &parentIndex = _target.parentIndex;
            const QAbstractItemModel *model = parentIndex.model();
            QPointF previousPoint = model->index(i - 1, VideoModel::PositionColumn, parentIndex)
                    .data(VideoModel::VideoSceneEditRole).toPointF();
            QPointF previousSpeed = model->index(i - 1, VideoModel::LSpeedColumn, parentIndex)
                    .data(VideoModel::VideoSceneEditRole).toPointF();
        _target.mutex.unlock();
        QPointF newSpeed = newPoint - previousPoint;
        QPointF newAccel = newSpeed - previousSpeed;

        emit instantGenerated(i, newPoint, newSpeed, newAccel);
        emit progressChanged(i);
    }
}

Target::Target(QObject *parent) :
    QObject(parent)
{}

void Target::storeInstant(int frame, const QPointF &p, const QPointF &s, const QPointF &a)
{
    QMutexLocker l(&mutex);

    if (!parentIndex.isValid()) {
        return;
    }

    int currentRow = model->rowCount(parentIndex);

    model->insertRow(currentRow, parentIndex);

    if (currentRow == 0) {
        model->insertColumns(0, VideoModel::LinearTrajectoryInstantColumnCount, parentIndex);
    }

    model->setData(model->index(currentRow, VideoModel::LFrameColumn, parentIndex), frame,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::PositionColumn, parentIndex), p,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::LSpeedColumn, parentIndex), s,
                   VideoModel::VideoSceneEditRole);
    model->setData(model->index(currentRow, VideoModel::LAccelerationColumn, parentIndex), a,
                   VideoModel::VideoSceneEditRole);
}
