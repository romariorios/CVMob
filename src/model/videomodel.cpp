/*
    CVMob - Motion capture program
    Copyright (C) 2013, 2014  The CVMob contributors

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

#include "videomodel.hpp"

#include <cmath>
#include <QMutexLocker>
#include <QSettings>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include <model/jobs/jobhandler.hpp>

// From the cosines law:
// c^2 = a^2 + b^2 - 2ab cos(t)
// cos(t) = (a^2 + b^2 - c^2) / 2ab

float angleFromPoints(const QPointF& c, const QPointF& e1, const QPointF& e2)
{
    float v1 = QLineF(e1, c).length();
    float v2 = QLineF(e2, c).length();
    float oc = QLineF(e1, e2).length();

    return acos(
        (v1 * v1 + v2 * v2 - oc * oc) / (2 * v1 * v2)
    );
}

VideoModel::VideoModel(QObject *parent) :
    QAbstractItemModel(parent),
    _indexesData(new QHash<QPair<int, int>, InternalData*>()),
    _cvmobVideoData(new QList<VideoModel::Video>)
{
}

VideoModel::~VideoModel()
{
    delete _indexesData;
    delete _cvmobVideoData;
}

QModelIndex VideoModel::index(int row, int column, const QModelIndex &parent) const
{
    auto ind = qMakePair(row, column);
    InternalData *parentData = static_cast<InternalData *>(parent.internalPointer());
    auto parentChildrenTable = parentData? &parentData->children : _indexesData;

    if (!parentChildrenTable->contains(ind)) {
        (*parentChildrenTable)[ind] = new InternalData(row, column, parentData);
    }

    return createIndex(row, column, (*parentChildrenTable)[ind]);
}

QModelIndex VideoModel::index(const QModelIndex &parent, const VideoModel::IndexPath &path) const
{
    auto currentIndex = parent;

    for (auto ind : path) {
        auto currentParent = currentIndex;
        currentIndex = index(ind.row, ind.column, currentParent);
    }

    return currentIndex;
}

QModelIndex VideoModel::index(const VideoModel::IndexPath &path) const
{
    return index(QModelIndex {}, path);
}

QModelIndex VideoModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    InternalData *childData = static_cast<InternalData *>(child.internalPointer());

    if (!childData->parent) {
        return QModelIndex();
    }

    return createIndex(childData->parent->row, childData->parent->column, childData->parent);
}

const VideoModel::IndexPath VideoModel::indexPath(const QModelIndex& index)
{
    if (!index.isValid()) {
        return IndexPath {};
    }

    IndexPathList pathList;

    for (auto i = index; i.isValid(); i = i.parent()) {
        pathList.prepend({ i.row(), i.column() });
    }

    return pathList.toVector();
}

QVariant VideoModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole ||
        !index.isValid()) {
        return QVariant();
    }

    if (!index.parent().isValid()) { // Level 0
        if (index.row() >= _cvmobVideoData->size()) {
            return QVariant();
        }

        const Video &currentVideo = _cvmobVideoData->at(index.row());

        switch (index.column()) {
        case FileNameCol:
            return currentVideo.fileName;
        case CurrentFrameCol:
            return currentVideo.currentFrame;
        case FrameDurationCol:
            return currentVideo.frameDuration;
        case FrameSizeCol:
            return currentVideo.frameSize;
        case PlayStatusCol:
            return currentVideo.playStatus;
        case CalibrationRatioCol:
            return currentVideo.calibration?
                *currentVideo.calibration : QVariant {};
        default:
            return QVariant();
        }
    } else if (!index.parent().parent().isValid()) { // Level 1
        if (index.parent().row() >= _cvmobVideoData->size()) {
            return QVariant();
        }

        Video &currentVideo = (*_cvmobVideoData)[index.parent().row()];

        if (index.parent().column() == AllFramesCol) {
            cv::Mat rawImg;

            QMutexLocker locker(&_streamLock);

            int oldFrame = currentVideo.streamFrame;
            int newFrame = index.row();

            if (newFrame == oldFrame) {
                return QVariant::fromValue(currentVideo.frameImage);
            } else if (newFrame == oldFrame + 1) {
                if (!currentVideo.videoStream.grab()) {
                    return QVariant();
                }
            } else {
                if (!currentVideo.videoStream.set(CV_CAP_PROP_POS_FRAMES, newFrame)) {
                    return QVariant();
                }
            }

            if (!currentVideo.videoStream.retrieve(rawImg)) {
                return QVariant(); // TODO report error
            }

            cvtColor(rawImg, rawImg, CV_BGR2RGB);
            currentVideo.frameImage = QImage(rawImg.data, rawImg.cols, rawImg.rows, QImage::Format_RGB888).copy();

            currentVideo.streamFrame = newFrame;

            locker.unlock();

            switch (index.column()) {
            case 0:
                return QVariant::fromValue(currentVideo.frameImage);
            default:
                return QVariant();
            }
        } else if (index.parent().column() == AllDistancesCol) {
            if (index.row() >= currentVideo.distances.size()) {
                return QVariant();
            }

            switch (index.column()) {
            case 0:
                return currentVideo.distances.at(index.row());
            default:
                return QVariant();
            }
        }
    } else if (!index.parent().parent().parent().isValid()) { // Level 2
        if (index.parent().parent().row() >= _cvmobVideoData->size()) {
            return QVariant();
        }

        const Video &currentVideo = _cvmobVideoData->at(index.parent().parent().row());

        if (index.parent().parent().column() == AllTrajectoriesCol) {
            if (index.parent().row() >= currentVideo.trajectories.size()) {
                return QVariant();
            }

            const Trajectory &currentTrajectory = currentVideo.trajectories.at(index.parent().row());

            if (index.row() >= currentTrajectory.instants.size()) {
                return QVariant();
            }

            const TrajectoryInstant &currentInstant = currentTrajectory.instants.at(index.row());

            switch (index.column()) {
            case LFrameCol:
                return currentInstant.frame;
            case PositionCol:
                return currentInstant.position * currentVideo.calibrationRatio();
            case LSpeedCol:
                return currentInstant.speed * currentVideo.calibrationRatio();
            case LAccelCol:
                return currentInstant.acceleration * currentVideo.calibrationRatio();
            }
        } else if (index.parent().parent().column() == AllAnglesCol) {
            if (index.parent().row() >= currentVideo.angles.size()) {
                return QVariant();
            }

            const Angle &currentAngle = currentVideo.angles.at(index.parent().row());

            if (index.row() >= currentAngle.instants.size()) {
                return QVariant();
            }

            const AngleInstant currentInstant = currentAngle.instants.at(index.row());

            switch (index.column()) {
            case AFrameCol:
                return currentInstant.frame;
            case AngleCol:
                return angleFromPoints(currentInstant.centralEdge,
                                    currentInstant.peripheralEdges.first,
                                    currentInstant.peripheralEdges.second);
            case ASpeedCol:
                return currentInstant.speed;
            case AAccelCol:
                return currentInstant.acceleration;
            case ACenterCol:
                return currentInstant.centralEdge * currentVideo.calibrationRatio();
            case AEdge1Col:
                return currentInstant.peripheralEdges.first * currentVideo.calibrationRatio();
            case AEdge2Col:
                return currentInstant.peripheralEdges.second * currentVideo.calibrationRatio();
            }
        }
    }

    return QVariant();
}

QVariant VideoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

int VideoModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) { // Level 0
        return VideoColCount;
    } else if (!parent.parent().isValid()) { // Level 1
        switch (parent.column()) {
        case AllFramesCol:
        case AllDistancesCol:
        case AllTrajectoriesCol:
        case AllAnglesCol:
            return 1;
        default:
            break;
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        switch (parent.parent().column()) {
        case AllTrajectoriesCol:
            return TrajectoryInstantColCount;
        case AllAnglesCol:
            return AngleInstantColCount;
        default:
            break;
        }
    }

    return 0;
}

int VideoModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) { // Level 0
        return _cvmobVideoData->size();
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.row() >= _cvmobVideoData->size()) {
            return 0;
        }

        const Video &currentVideo = _cvmobVideoData->at(parent.row());

        switch (parent.column()) {
        case AllFramesCol:
            return currentVideo.frameCount;
        case AllDistancesCol:
            return currentVideo.distances.size();
        case AllTrajectoriesCol:
            return currentVideo.trajectories.size();
        case AllAnglesCol:
            return currentVideo.angles.size();
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        if (parent.parent().row() >= _cvmobVideoData->size()) {
            return 0;
        }

        const Video &currentVideo = _cvmobVideoData->at(parent.parent().row());

        if (parent.parent().column() == AllTrajectoriesCol) {
            const Trajectory &currentTrajectory = currentVideo.trajectories.at(parent.row());

            return currentTrajectory.instants.size();
        } else if (parent.parent().column() == AllAnglesCol) {
            const Angle &currentTrajectory = currentVideo.angles.at(parent.row());

            return currentTrajectory.instants.size();
        }
    }

    return 0;
}

Qt::ItemFlags VideoModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool VideoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole ||
        !index.isValid() ||
        !value.isValid()) {
        return false;
    }

    if (!index.parent().isValid()) { // Level 0
        if (index.row() >= _cvmobVideoData->size()) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[index.row()];

        switch (index.column()) {
        case FileNameCol:
            currentVideo.fileName = value.toString();
            break;
        case CurrentFrameCol:
            currentVideo.currentFrame = value.toInt();
            if (currentVideo.jobHandler) {
                currentVideo.jobHandler->setVideoFrame(value.toInt());
            }
            break;
        case FrameDurationCol:
            currentVideo.frameDuration = value.toInt();
            break;
        case FrameSizeCol:
            currentVideo.frameSize = value.toSizeF();
            break;
        case PlayStatusCol:
            currentVideo.playStatus = value.toBool();
            currentVideo.jobHandler->setPlayStatus(value.toBool());
            break;
        case CalibrationRatioCol:
            if (value.isValid()) {
                currentVideo.setCalibrationRatio(value.toDouble());
            } else {
                currentVideo.unsetCalibration();
            }
        default:
            return false;
        }
    } else if (!index.parent().parent().isValid()) { // Level 1
        if (index.parent().column() != AllDistancesCol ||
            index.parent().row() >= _cvmobVideoData->size() ||
            index.column() != 0) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[index.parent().row()];

        if (index.row() >= currentVideo.distances.size()) {
            return false;
        }

        currentVideo.distances[index.row()] = value.toLineF();
    } else if (!index.parent().parent().parent().isValid()) { // Level 2
        if (index.parent().parent().row() >= _cvmobVideoData->size()) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[index.parent().parent().row()];

        if (index.parent().parent().column() == AllTrajectoriesCol) {
            if (index.parent().row() >= currentVideo.trajectories.size()) {
                return false;
            }

            Trajectory &currentTrajectory = currentVideo.trajectories[index.parent().row()];

            if (index.row() >= currentTrajectory.instants.size()) {
                return false;
            }

            TrajectoryInstant &currentInstant = currentTrajectory.instants[index.row()];

            switch (index.column()) {
            case LFrameCol:
                currentInstant.frame = value.toInt();
                break;
            case PositionCol:
                currentInstant.position = value.toPointF() / currentVideo.calibrationRatio();
                break;
            case LSpeedCol:
                currentInstant.speed = value.toPointF() / currentVideo.calibrationRatio();
                break;
            case LAccelCol:
                currentInstant.acceleration = value.toPointF() / currentVideo.calibrationRatio();
                break;
            default:
                return false;
            }
        } else if (index.parent().parent().column() == AllAnglesCol) {
            if (index.parent().row() >= currentVideo.angles.size()) {
                return false;
            }

            Angle &currentTrajectory = currentVideo.angles[index.parent().row()];

            if (index.row() >= currentTrajectory.instants.size()) {
                return false;
            }

            AngleInstant &currentInstant = currentTrajectory.instants[index.row()];
            QModelIndex angleIndex = this->index(index.row(), AngleCol, index.parent());

            switch (index.column()) {
            case AFrameCol:
                currentInstant.frame = value.toInt();
                break;

            case ASpeedCol:
                currentInstant.speed = value.toFloat();
                break;
            case AAccelCol:
                currentInstant.acceleration = value.toFloat();
                break;
            case ACenterCol:
                currentInstant.centralEdge = value.toPointF() / currentVideo.calibrationRatio();
                emit dataChanged(angleIndex, angleIndex);
                break;
            case AEdge1Col:
                currentInstant.peripheralEdges.first = value.toPointF() / currentVideo.calibrationRatio();
                emit dataChanged(angleIndex, angleIndex);
                break;
            case AEdge2Col:
                currentInstant.peripheralEdges.second = value.toPointF() / currentVideo.calibrationRatio();
                emit dataChanged(angleIndex, angleIndex);
                break;
            default:
                return false;
            }
        } else {
            return false;
        }
    }

    emit dataChanged(index, index);
    return true;
}

template <class T> bool VideoModel::checkAndInsertRowsIn(QList<T> &l,
                                                              int row,
                                                              int count,
                                                              const QModelIndex &parent)
{
    int first = row;
    int last = row + count - 1;

    if (first > l.size()) {
        return false;
    }

    beginInsertRows(parent, first, last);

    for (int i = 0; i < count; ++i) {
        l.insert(row, *new T);
    }

    endInsertRows();

    return true;
}

bool VideoModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid()) { // Level 0
        return checkAndInsertRowsIn(*_cvmobVideoData, row, count);
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.row() >= _cvmobVideoData->size()) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[parent.row()];

        switch (parent.column()) {
        case AllFramesCol:
            return false; // Inserting frames is not possible
        case AllDistancesCol:
            return checkAndInsertRowsIn(currentVideo.distances, row, count, parent);
        case AllTrajectoriesCol:
            return checkAndInsertRowsIn(currentVideo.trajectories, row, count, parent);
        case AllAnglesCol:
            return checkAndInsertRowsIn(currentVideo.angles, row, count, parent);
        default:
            return false;
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        if (parent.parent().row() >= _cvmobVideoData->size()) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[parent.parent().row()];

        if (parent.parent().column() == AllTrajectoriesCol) {
            if (parent.row() >= currentVideo.trajectories.size()) {
                return false;
            }

            return checkAndInsertRowsIn(
                        currentVideo.trajectories[parent.row()].instants,
                        row,
                        count,
                        parent);
        } else if (parent.parent().column() == AllAnglesCol) {
            if (parent.row() >= currentVideo.angles.size()) {
                return false;
            }

            return checkAndInsertRowsIn(
                        currentVideo.angles[parent.row()].instants,
                        row,
                        count,
                        parent);
        }
    }

    return false;
}

template <class T> bool VideoModel::checkAndRemoveRowsFrom(QList<T> &l,
                                                                int row,
                                                                int count,
                                                                const QModelIndex &parent)
{
    int first = row;
    int last = row + count - 1;

    if (last >= l.size()) {
        return false;
    }

    // TODO remove index data from _indexesData

    beginRemoveRows(parent, first, last);

    for (int i = 0; i < count; ++i) {
        l.removeAt(row);
    }

    endRemoveRows();

    return true;
}

bool VideoModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid()) { // Level 0
        return checkAndRemoveRowsFrom(*_cvmobVideoData, row, count);
    } else if (!parent.parent().isValid()) { // Level 1
        if (parent.row() >= _cvmobVideoData->size()) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[parent.row()];

        switch (parent.column()) {
        case AllFramesCol:
            return false; // Inserting frames is not possible
        case AllDistancesCol:
            return checkAndRemoveRowsFrom(currentVideo.distances, row, count, parent);
        case AllTrajectoriesCol:
            return checkAndRemoveRowsFrom(currentVideo.trajectories, row, count, parent);
        case AllAnglesCol:
            return checkAndRemoveRowsFrom(currentVideo.angles, row, count, parent);
        default:
            return false;
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        if (parent.parent().row() >= _cvmobVideoData->size()) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[parent.parent().row()];

        if (parent.parent().column() == AllTrajectoriesCol) {
            if (parent.row() >= currentVideo.trajectories.size()) {
                return false;
            }

            return checkAndRemoveRowsFrom(
                        currentVideo.trajectories[parent.row()].instants,
                        row,
                        count,
                        parent);
        } else if (parent.parent().column() == AllAnglesCol) {
            if (parent.row() >= currentVideo.angles.size()) {
                return false;
            }

            return checkAndRemoveRowsFrom(
                        currentVideo.angles[parent.row()].instants,
                        row,
                        count,
                        parent);
        }
    }

    return false;
}

bool VideoModel::openVideo(const QString& path)
{
    insertRow(rowCount());
    Video &newVideo = _cvmobVideoData->last();
    cv::VideoCapture &videoStream = newVideo.videoStream;

    if (!videoStream.open(path.toUtf8().constData()) ||
        videoStream.get(CV_CAP_PROP_FRAME_COUNT) == 0) {
        removeRow(rowCount() - 1);
        return false;
    }

    QModelIndex fileNameIndex = index(rowCount() - 1, FileNameCol);
    setData(fileNameIndex, path);

    QModelIndex currentFrameIndex = index(fileNameIndex.row(), CurrentFrameCol);
    setData(currentFrameIndex, 0);

    QModelIndex frameDurationIndex = index(fileNameIndex.row(), FrameDurationCol);
    setData(frameDurationIndex, 1000 * (1 / videoStream.get(CV_CAP_PROP_FPS)));

    QModelIndex frameSizeIndex = index(fileNameIndex.row(), FrameSizeCol);
    setData(
        frameSizeIndex,
        QSizeF(
            videoStream.get(CV_CAP_PROP_FRAME_WIDTH),
            videoStream.get(CV_CAP_PROP_FRAME_HEIGHT)
        )
    );

    QSettings s;
    const auto winSize = s.value("video/searchWindowSize", 21).toInt();

    newVideo.frameCount = videoStream.get(CV_CAP_PROP_FRAME_COUNT);
    newVideo.jobHandler = new JobHandler(fileNameIndex.row(), this);
    newVideo.jobHandler->setWindowSize(QSize { winSize, winSize });

    return true;
}

JobHandler* VideoModel::jobHandlerForVideo(int videoRow) const
{
    return _cvmobVideoData->at(videoRow).jobHandler;
}

void VideoModel::createDistance(const QLineF &l, const QModelIndex &distancesIndex)
{
    int ind = rowCount(distancesIndex);
    insertRow(ind, distancesIndex);
    setData(index(ind, 0, distancesIndex), l);
}

void VideoModel::createDistance(const QLineF &l, int videoRow)
{
    createDistance(l, index(videoRow, AllDistancesCol));
}

void VideoModel::createDistance(const QPointF& p1,
                                const QPointF& p2,
                                const QModelIndex &distancesIndex)
{
    createDistance(QLineF(p1, p2), distancesIndex);
}

void VideoModel::createDistance(const QPointF& p1, const QPointF& p2, int videoRow)
{
    createDistance(p1, p2, index(videoRow, AllDistancesCol));
}

AngleCalcJob* VideoModel::calculateAngle(const QVector< QPointF >& anglePoints, int frame,
                                         int videoRow, const QSize& windowSize,
                                         VideoModel::CalculationFlags flags)
{
    if (flags != FromHereOnwards) {
        // TODO implement backwards calculation
        return 0;
    }

    int startFrame = frame;
    int endFrame = rowCount(index(videoRow, AllFramesCol)) - 1;

    QModelIndex anglesIndex = index(videoRow, AllAnglesCol);

    int angleRow = rowCount(anglesIndex);
    insertRow(angleRow, anglesIndex);

    QModelIndex currentAngleIndex = index(angleRow, 0, anglesIndex);

    auto job = new AngleCalcJob(anglePoints, startFrame, endFrame, videoRow, this);
    job->setTarget(currentAngleIndex);

    return job;
}

TrajectoryCalcJob *VideoModel::calculateTrajectory(const QPointF &p, int frame,
                                                                 int videoRow,
                                                                 const QSize &windowSize,
                                                                 CalculationFlags flags)
{
    int startFrame = frame;
    int endFrame = frame;

    if (flags & FromHereOnwards) {
        endFrame = rowCount(index(videoRow, AllFramesCol)) - 1;
    }

    if (flags & FromHereBackwards) {
        startFrame = 0;
    }

    QModelIndex trajectoriesIndex = index(videoRow, AllTrajectoriesCol);

    int trajectoryRow = rowCount(trajectoriesIndex);
    insertRow(trajectoryRow, trajectoriesIndex);

    QModelIndex currentTrajectoryIndex = index(trajectoryRow, 0, trajectoriesIndex);

    TrajectoryCalcJob *job =
            new TrajectoryCalcJob(p,
                                        startFrame,
                                        endFrame,
                                        videoRow,
                                        this);
    job->setTarget(currentTrajectoryIndex);

    return job;
}

void VideoModel::updateSettings()
{
    const auto winSize = QSettings {}.value("video/searchWindowSize", 21).toInt();

    for (auto &video : *_cvmobVideoData) {
        video.jobHandler->setWindowSize(QSize { winSize, winSize });
    }
}

VideoModel::Video::~Video()
{
    delete jobHandler;
}
