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

#include "videomodel.hpp"

#include <cmath>
#include <QMutexLocker>
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
        case FileNameColumn:
            return currentVideo.fileName;
        case CurrentFrameColumn:
            return currentVideo.currentFrame;
        case FrameDurationColumn:
            return currentVideo.frameDuration;
        case FrameSizeColumn:
            return currentVideo.frameSize;
        default:
            return QVariant();
        }
    } else if (!index.parent().parent().isValid()) { // Level 1
        if (index.parent().row() >= _cvmobVideoData->size()) {
            return QVariant();
        }

        Video &currentVideo = (*_cvmobVideoData)[index.parent().row()];
        
        if (index.parent().column() == FramesColumn) {
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
        } else if (index.parent().column() == DistancesColumn) {
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
        
        if (index.parent().parent().column() == TrajectoriesColumn) {
            if (index.parent().row() >= currentVideo.trajectories.size()) {
                return QVariant();
            }

            const Trajectory &currentTrajectory = currentVideo.trajectories.at(index.parent().row());

            if (index.row() >= currentTrajectory.instants.size()) {
                return QVariant();
            }

            const TrajectoryInstant &currentInstant = currentTrajectory.instants.at(index.row());

            switch (index.column()) {
            case LFrameColumn:
                return currentInstant.frame;
            case PositionColumn:
                return currentInstant.position;
            case LSpeedColumn:
                return currentInstant.speed;
            case LAccelerationColumn:
                return currentInstant.acceleration;
            }
        } else if (index.parent().parent().column() == AnglesColumn) {
            if (index.parent().row() >= currentVideo.angles.size()) {
                return QVariant();
            }
            
            const Angle &currentAngle = currentVideo.angles.at(index.parent().row());

            if (index.row() >= currentAngle.instants.size()) {
                return QVariant();
            }

            const AngleInstant currentInstant = currentAngle.instants.at(index.row());

            switch (index.column()) {
            case AFrameColumn:
                return currentInstant.frame;
            case AngleColumn:
                return angleFromPoints(currentInstant.centralEdge,
                                    currentInstant.peripheralEdges.first,
                                    currentInstant.peripheralEdges.second);
            case ASpeedColumn:
                return currentInstant.speed;
            case AAccelerationColumn:
                return currentInstant.acceleration;
            case CentralEdgeColumn:
                return currentInstant.centralEdge;
            case PeripheralEdge1Column:
                return currentInstant.peripheralEdges.first;
            case PeripheralEdge2Column:
                return currentInstant.peripheralEdges.second;
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
        return VideoColumnCount;
    } else if (!parent.parent().isValid()) { // Level 1
        switch (parent.column()) {
        case FramesColumn:
        case DistancesColumn:
        case TrajectoriesColumn:
        case AnglesColumn:
            return 1;
        default:
            break;
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        switch (parent.parent().column()) {
        case TrajectoriesColumn:
            return TrajectoryInstantColumnCount;
        case AnglesColumn:
            return AngleInstantColumnCount;
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
        case FramesColumn:
            return currentVideo.frameCount;
        case DistancesColumn:
            return currentVideo.distances.size();
        case TrajectoriesColumn:
            return currentVideo.trajectories.size();
        case AnglesColumn:
            return currentVideo.angles.size();
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        if (parent.parent().row() >= _cvmobVideoData->size()) {
            return 0;
        }
        
        const Video &currentVideo = _cvmobVideoData->at(parent.parent().row());
        
        if (parent.parent().column() == TrajectoriesColumn) {
            const Trajectory &currentTrajectory = currentVideo.trajectories.at(parent.row());

            return currentTrajectory.instants.size();
        } else if (parent.parent().column() == AnglesColumn) {
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
        case FileNameColumn:
            currentVideo.fileName = value.toString();
            break;
        case CurrentFrameColumn:
            currentVideo.currentFrame = value.toInt();
            break;
        case FrameDurationColumn:
            currentVideo.frameDuration = value.toInt();
            break;
        case FrameSizeColumn:
            currentVideo.frameSize = value.toSizeF();
            break;
        default:
            return false;
        }
    } else if (!index.parent().parent().isValid()) { // Level 1
        if (index.parent().column() != DistancesColumn ||
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

        if (index.parent().parent().column() == TrajectoriesColumn) {
            if (index.parent().row() >= currentVideo.trajectories.size()) {
                return false;
            }

            Trajectory &currentTrajectory = currentVideo.trajectories[index.parent().row()];

            if (index.row() >= currentTrajectory.instants.size()) {
                return false;
            }

            TrajectoryInstant &currentInstant = currentTrajectory.instants[index.row()];

            switch (index.column()) {
            case LFrameColumn:
                currentInstant.frame = value.toInt();
                break;
            case PositionColumn:
                currentInstant.position = value.toPointF();
                break;
            case LSpeedColumn:
                currentInstant.speed = value.toPointF();
                break;
            case LAccelerationColumn:
                currentInstant.acceleration = value.toPointF();
                break;
            default:
                return false;
            }
        } else if (index.parent().parent().column() == AnglesColumn) {
            if (index.parent().row() >= currentVideo.angles.size()) {
                return false;
            }

            Angle &currentTrajectory = currentVideo.angles[index.parent().row()];

            if (index.row() >= currentTrajectory.instants.size()) {
                return false;
            }

            AngleInstant &currentInstant = currentTrajectory.instants[index.row()];
            QModelIndex angleIndex = this->index(index.row(), AngleColumn, index.parent());
            
            switch (index.column()) {
            case AFrameColumn:
                currentInstant.frame = value.toInt();
                break;
        
            case ASpeedColumn:
                currentInstant.speed = value.toFloat();
                break;
            case AAccelerationColumn:
                currentInstant.acceleration = value.toFloat();
                break;
            case CentralEdgeColumn:
                currentInstant.centralEdge = value.toPointF();
                emit dataChanged(angleIndex, angleIndex);
                break;
            case PeripheralEdge1Column:
                currentInstant.peripheralEdges.first = value.toPointF();
                emit dataChanged(angleIndex, angleIndex);
                break;
            case PeripheralEdge2Column:
                currentInstant.peripheralEdges.second = value.toPointF();
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
        case FramesColumn:
            return false; // Inserting frames is not possible
        case DistancesColumn:
            return checkAndInsertRowsIn(currentVideo.distances, row, count, parent);
        case TrajectoriesColumn:
            return checkAndInsertRowsIn(currentVideo.trajectories, row, count, parent);
        case AnglesColumn:
            return checkAndInsertRowsIn(currentVideo.angles, row, count, parent);
        default:
            return false;
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        if (parent.parent().row() >= _cvmobVideoData->size()) {
            return false;
        }
        
        Video &currentVideo = (*_cvmobVideoData)[parent.parent().row()];
        
        if (parent.parent().column() == TrajectoriesColumn) {
            if (parent.row() >= currentVideo.trajectories.size()) {
                return false;
            }
            
            return checkAndInsertRowsIn(
                        currentVideo.trajectories[parent.row()].instants,
                        row,
                        count,
                        parent);
        } else if (parent.parent().column() == AnglesColumn) {
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
        case FramesColumn:
            return false; // Inserting frames is not possible
        case DistancesColumn:
            return checkAndRemoveRowsFrom(currentVideo.distances, row, count, parent);
        case TrajectoriesColumn:
            return checkAndRemoveRowsFrom(currentVideo.trajectories, row, count, parent);
        case AnglesColumn:
            return checkAndRemoveRowsFrom(currentVideo.angles, row, count, parent);
        default:
            return false;
        }
    } else if (!parent.parent().parent().isValid()) { // Level 2
        if (parent.parent().row() >= _cvmobVideoData->size()) {
            return false;
        }
        
        Video &currentVideo = (*_cvmobVideoData)[parent.parent().row()];
        
        if (parent.parent().column() == TrajectoriesColumn) {
            if (parent.row() >= currentVideo.trajectories.size()) {
                return false;
            }
            
            return checkAndRemoveRowsFrom(
                        currentVideo.trajectories[parent.row()].instants,
                        row,
                        count,
                        parent);
        } else if (parent.parent().column() == AnglesColumn) {
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
    
    QModelIndex fileNameIndex = index(rowCount() - 1, FileNameColumn);
    setData(fileNameIndex, path);
    
    QModelIndex currentFrameIndex = index(fileNameIndex.row(), CurrentFrameColumn);
    setData(currentFrameIndex, 0);
    
    QModelIndex frameDurationIndex = index(fileNameIndex.row(), FrameDurationColumn);
    setData(frameDurationIndex, 1000 * (1 / videoStream.get(CV_CAP_PROP_FPS)));
    
    QModelIndex frameSizeIndex = index(fileNameIndex.row(), FrameSizeColumn);
    setData(
        frameSizeIndex,
        QSizeF(
            videoStream.get(CV_CAP_PROP_FRAME_WIDTH),
            videoStream.get(CV_CAP_PROP_FRAME_HEIGHT)
        )
    );
    
    newVideo.frameCount = videoStream.get(CV_CAP_PROP_FRAME_COUNT);
    newVideo.jobHandler = new JobHandler(fileNameIndex.row(), this);
    newVideo.jobHandler->setWindowSize(QSize(21, 21));
    
    return true;
}

JobHandler* VideoModel::jobHandlerForVideo(int videoRow)
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
    createDistance(l, index(videoRow, DistancesColumn));
}

void VideoModel::createDistance(const QPointF& p1,
                                const QPointF& p2,
                                const QModelIndex &distancesIndex)
{
    createDistance(QLineF(p1, p2), distancesIndex);
}

void VideoModel::createDistance(const QPointF& p1, const QPointF& p2, int videoRow)
{
    createDistance(p1, p2, index(videoRow, DistancesColumn));
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
    int endFrame = rowCount(index(videoRow, FramesColumn)) - 1;
    
    QModelIndex anglesIndex = index(videoRow, AnglesColumn);

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
        endFrame = rowCount(index(videoRow, FramesColumn)) - 1;
    }

    if (flags & FromHereBackwards) {
        startFrame = 0;
    }

    QModelIndex trajectoriesIndex = index(videoRow, TrajectoriesColumn);

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

VideoModel::Video::~Video()
{
    delete jobHandler;
}
