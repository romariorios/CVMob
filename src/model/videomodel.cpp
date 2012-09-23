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

#include "videomodel.hpp"

#include <cmath>
#include <opencv2/imgproc/imgproc.hpp>

#include <QDebug>

VideoModel::VideoModel(QObject *parent) :
    QAbstractItemModel(parent),
    _cvmobVideoData(new QList<VideoModel::Video>)
{
}

QModelIndex VideoModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(row, column, new InternalData(VideoData, row));
    }

    InternalData *parentInternalPointer = static_cast<InternalData *>(parent.internalPointer());
    InternalDataType parentInternalPointerType = parentInternalPointer->type;
    int parentColumn = parent.column();

    switch (parentInternalPointerType) {
    case VideoData:
        switch (parentColumn) {
        case FramesColumn:
            return createIndex(row, column, new InternalData(FrameData, row, parentInternalPointer));
        case DistancesColumn:
            return createIndex(row, column, new InternalData(DistanceData, row, parentInternalPointer));
        case LinearTrajectoriesColumn:
            return createIndex(row, column, new InternalData(LinearTrajectoryData, row, parentInternalPointer));
        case AngularTrajectoriesColumn:
            return createIndex(row, column, new InternalData(AngularTrajectoryData, row, parentInternalPointer));
        default:
            break;
        }
    case LinearTrajectoryData:
        if (parentColumn == 0) {
            return createIndex(row, column, new InternalData(LinearTrajectoryInstantData, row, parentInternalPointer));
        }
    case AngularTrajectoryData:
        if (parentColumn == 0) {
            return createIndex(row, column, new InternalData(AngularTrajectoryInstantData, row, parentInternalPointer));
        }
    default:
        break;
    }

    return QModelIndex();
}

QModelIndex VideoModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    InternalData *data = static_cast<InternalData *>(child.internalPointer());

    if (!data->parent) {
        return QModelIndex();
    }

    int parentColumn;

    switch (data->type) {
    case FrameData:
        parentColumn = FramesColumn;
        break;
    case DistanceData:
        parentColumn = DistancesColumn;
        break;
    case LinearTrajectoryData:
        parentColumn = LinearTrajectoriesColumn;
        break;
    case AngularTrajectoryData:
        parentColumn = AngularTrajectoriesColumn;
        break;
    case LinearTrajectoryInstantData:
    case AngularTrajectoryInstantData:
        parentColumn = 0;
        break;
    default:
        return QModelIndex();
    }

    return createIndex(data->parent->row, parentColumn, data->parent);
}

QVariant VideoModel::data(const QModelIndex &index, int role) const
{
    if ((role != VideoSceneRole &&
        role != VideoSceneEditRole &&
        role != Qt::DisplayRole) ||
        !index.isValid()) {
        return QVariant();
    }

    InternalData *internalPointer = static_cast<InternalData *>(index.internalPointer());

    if (internalPointer->type == VideoData) {
        if (index.row() >= _cvmobVideoData->size()) {
            return QVariant();
        }

        Video currentVideo = _cvmobVideoData->at(index.row());

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
    } else {
        if (!internalPointer->parent ||
            internalPointer->parent->row >= _cvmobVideoData->size()) {
            return QVariant();
        }

        Video currentVideo = _cvmobVideoData->at(internalPointer->parent->row);

        if (internalPointer->type == FrameData) {
            cv::Mat rawImg;
            
            currentVideo.videoStream.set(CV_CAP_PROP_POS_FRAMES, index.row());
            if (!currentVideo.videoStream.read(rawImg)) {
                return QVariant();
            }
            
            cvtColor(rawImg, rawImg, CV_BGR2RGB);
            const QImage frame(rawImg.data, rawImg.cols, rawImg.rows, QImage::Format_RGB888);

            switch (index.column()) {
            case 0:
                return QVariant::fromValue(frame);
            default:
                return QVariant();
            }
        } else if (internalPointer->type == DistanceData) {
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

        InternalData *parentPointer = internalPointer->parent;

        if (!parentPointer &&
            !parentPointer->parent) {
            return QVariant();
        }

        if (internalPointer->type == LinearTrajectoryInstantData) {
            if (parentPointer->row >= currentVideo.linearTrajectories.size()) {
                return QVariant();
            }

            LinearTrajectory currentTrajectory = currentVideo.linearTrajectories.at(parentPointer->row);

            if (index.row() >= currentTrajectory.instants.size()) {
                return QVariant();
            }

            LinearTrajectoryInstant currentInstant = currentTrajectory.instants.at(index.row());

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
        } else if (internalPointer->type == AngularTrajectoryInstantData) {
            if (parentPointer->row >= currentVideo.angularTrajectories.size()) {
                return QVariant();
            }

            AngularTrajectory currentTrajectory = currentVideo.angularTrajectories.at(parentPointer->row);

            if (index.row() >= currentTrajectory.instants.size()) {
                return QVariant();
            }

            AngularTrajectoryInstant currentInstant = currentTrajectory.instants.at(index.row());

            switch (index.column()) {
            case AFrameColumn:
                return currentInstant.frame;
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
    if (role != VideoSceneRole ||
        role != VideoSceneEditRole ||
        role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case FileNameColumn:
            return tr("File name");
        case CurrentFrameColumn:
            return tr("Current frame");
        case FrameDurationColumn:
            return tr("Frame duration");
        case FrameSizeColumn:
            return tr("Frame size");
        }
    }

    return QVariant();
}

int VideoModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return VideoColumnCount;
    }

    switch (static_cast<InternalData *>(parent.internalPointer())->type) {
    case VideoData:
        switch (parent.column()) {
        case FramesColumn:
        case DistancesColumn:
        case LinearTrajectoriesColumn:
        case AngularTrajectoriesColumn:
            return 1;
        default:
            break;
        }
    case LinearTrajectoryData:
        return LinearTrajectoryInstantColumnCount;
    case AngularTrajectoryData:
        return AngularTrajectoryInstantColumnCount;
    default:
        break;
    }

    return 0;
}

int VideoModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return _cvmobVideoData->size();
    }

    InternalData *internalPointer = static_cast<InternalData *>(parent.internalPointer());
    const Video &parentVideo = _cvmobVideoData->at(internalPointer->row);

    if (internalPointer->type == VideoData) {
        switch (parent.column()) {
        case FramesColumn:
            return parentVideo.frameCount;
        case DistancesColumn:
            return parentVideo.distances.size();
        case LinearTrajectoriesColumn:
            return parentVideo.linearTrajectories.size();
        case AngularTrajectoriesColumn:
            return parentVideo.angularTrajectories.size();
        }
    } else if (internalPointer->parent) {
        InternalData *grandpaPointer = internalPointer->parent;

        if (internalPointer->type == LinearTrajectoryData) {
            LinearTrajectory currentTrajectory = parentVideo.linearTrajectories.at(grandpaPointer->row);

            return currentTrajectory.instants.size();
        } else if (internalPointer->type == AngularTrajectoryData) {
            AngularTrajectory currentTrajectory = parentVideo.angularTrajectories.at(grandpaPointer->row);

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
    if (role != VideoSceneEditRole ||
        !index.isValid() ||
        !value.isValid()) {
        return false;
    }

    InternalData *internalPointer = static_cast<InternalData *>(index.internalPointer());

    if (internalPointer->type == VideoData) {
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
        }
    } else {
        if (!internalPointer->parent ||
            internalPointer->parent->row >= _cvmobVideoData->size()) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[internalPointer->parent->row];

        if (internalPointer->type == FrameData) {
            return false; // Setting a frame is not possible
        } else if (internalPointer->type == DistanceData) {
            if (index.row() >= currentVideo.distances.size()) {
                return false;
            }

            switch (index.column()) {
            case 0:
                currentVideo.distances[index.row()] = value.toLineF();
                break;
            }
        } else {
            InternalData *parentPointer = internalPointer->parent;

            if (!parentPointer &&
                !parentPointer->parent) {
                return false;
            }

            if (internalPointer->type == LinearTrajectoryInstantData) {
                if (parentPointer->row >= currentVideo.linearTrajectories.size()) {
                    return false;
                }

                LinearTrajectory &currentTrajectory = currentVideo.linearTrajectories[parentPointer->row];

                if (index.row() >= currentTrajectory.instants.size()) {
                    return false;
                }

                LinearTrajectoryInstant &currentInstant = currentTrajectory.instants[index.row()];

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
                }
            } else if (internalPointer->type == AngularTrajectoryInstantData) {
                if (parentPointer->row >= currentVideo.angularTrajectories.size()) {
                    return false;
                }

                AngularTrajectory &currentTrajectory = currentVideo.angularTrajectories[parentPointer->row];

                if (index.row() >= currentTrajectory.instants.size()) {
                    return false;
                }

                AngularTrajectoryInstant &currentInstant = currentTrajectory.instants[index.row()];

                switch (index.column()) {
                case AFrameColumn:
                    currentInstant.frame = value.toInt();
                    break;
                case ASpeedColumn:
                    currentInstant.speed = value.toInt();
                    break;
                case AAccelerationColumn:
                    currentInstant.acceleration = value.toInt();
                    break;
                case CentralEdgeColumn:
                    currentInstant.centralEdge = value.toPointF();
                    break;
                case PeripheralEdge1Column:
                    currentInstant.peripheralEdges.first = value.toPointF();
                    break;
                case PeripheralEdge2Column:
                    currentInstant.peripheralEdges.second = value.toPointF();
                    break;
                }
            } else {
                return false;
            }
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
    if (!parent.isValid()) {
        return checkAndInsertRowsIn<Video>(*_cvmobVideoData, row, count);
    }

    InternalData *parentPointer = static_cast<InternalData *>(parent.internalPointer());
    Video &currentVideo = (*_cvmobVideoData)[parentPointer->row];

    if (parentPointer->type == VideoData) {
        switch (parent.column()) {
        case FramesColumn:
            return false; // Inserting frames is not possible
        case DistancesColumn:
            return checkAndInsertRowsIn<QLineF>(currentVideo.distances, row, count, parent);
        case LinearTrajectoriesColumn:
            return checkAndInsertRowsIn<LinearTrajectory>(currentVideo.linearTrajectories, row, count, parent);
        case AngularTrajectoriesColumn:
            return checkAndInsertRowsIn<AngularTrajectory>(currentVideo.angularTrajectories, row, count, parent);
        default:
            return false;
        }
    }
    else if (parent.column() != 0) {
        return false;
    } else {
        InternalData *grandparentPointer = parentPointer->parent;

        if (!grandparentPointer) {
            return false;
        }

        if (parentPointer->type == LinearTrajectoryData) {
            return checkAndInsertRowsIn<LinearTrajectoryInstant>(
                        currentVideo.linearTrajectories[grandparentPointer->row].instants,
                        row,
                        count,
                        parent);
        } else if (parentPointer->type == AngularTrajectoryData) {
            return checkAndInsertRowsIn<AngularTrajectoryInstant>(
                        currentVideo.angularTrajectories[grandparentPointer->row].instants,
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

    beginRemoveRows(parent, first, last);

    for (int i = 0; i < count; ++i) {
        l.removeAt(row);
    }

    endRemoveRows();

    return true;
}

bool VideoModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid()) {
        return checkAndRemoveRowsFrom<Video>(*_cvmobVideoData, row, count);
    }

    InternalData *parentPointer = static_cast<InternalData *>(parent.internalPointer());
    Video &currentVideo = (*_cvmobVideoData)[parentPointer->row];

    if (parentPointer->type == VideoData) {
        switch (parent.column()) {
        case FramesColumn:
            return false; // Removing frames is not possible
        case DistancesColumn:
            return checkAndRemoveRowsFrom<QLineF>(currentVideo.distances, row, count, parent);
        case LinearTrajectoriesColumn:
            return checkAndRemoveRowsFrom<LinearTrajectory>(currentVideo.linearTrajectories, row, count, parent);
        case AngularTrajectoriesColumn:
            return checkAndRemoveRowsFrom<AngularTrajectory>(currentVideo.angularTrajectories, row, count, parent);
        default:
            return false;
        }
    }
    else if (parent.column() != 0) {
        return false;
    } else {
        InternalData *grandparentPointer = parentPointer->parent;

        if (!grandparentPointer) {
            return false;
        }

        if (parentPointer->type == LinearTrajectoryData) {
            return checkAndRemoveRowsFrom<LinearTrajectoryInstant>(
                        currentVideo.linearTrajectories[grandparentPointer->row].instants,
                        row,
                        count,
                        parent);
        } else if (parentPointer->type == AngularTrajectoryData) {
            return checkAndRemoveRowsFrom<AngularTrajectoryInstant>(
                        currentVideo.angularTrajectories[grandparentPointer->row].instants,
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
    
    if (!videoStream.open(path.toUtf8().constData())) {
        removeRow(rowCount() - 1);
        return false;
    }
    
    QModelIndex fileNameIndex = index(rowCount() - 1, FileNameColumn);
    setData(fileNameIndex, path, VideoSceneEditRole);
    
    QModelIndex currentFrameIndex = index(fileNameIndex.row(), CurrentFrameColumn);
    setData(currentFrameIndex, 0, VideoSceneEditRole);
    
    QModelIndex frameDurationIndex = index(fileNameIndex.row(), FrameDurationColumn);
    setData(frameDurationIndex,
            1000 * (1 / videoStream.get(CV_CAP_PROP_FPS)),
            VideoSceneEditRole);
    
    QModelIndex frameSizeIndex = index(fileNameIndex.row(), FrameSizeColumn);
    setData(
        frameSizeIndex,
        QSizeF(
            videoStream.get(CV_CAP_PROP_FRAME_WIDTH),
            videoStream.get(CV_CAP_PROP_FRAME_HEIGHT)
        ), 
        VideoSceneEditRole
    );
    
    newVideo.frameCount = videoStream.get(CV_CAP_PROP_FRAME_COUNT);
    
    return true;
}

void VideoModel::createDistance(const QPointF& p1,
                                const QPointF& p2,
                                const QModelIndex &distancesIndex)
{
    int ind = rowCount(distancesIndex);
    insertRow(ind, distancesIndex);
    setData(index(ind, 0, distancesIndex), QLineF(p1, p2), VideoSceneEditRole);
}

void VideoModel::createDistance(const QPointF& p1, const QPointF& p2, int videoRow)
{
    createDistance(p1, p2, index(videoRow, DistancesColumn));
}
