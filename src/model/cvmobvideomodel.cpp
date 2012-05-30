#include "cvmobvideomodel.hpp"

#include <cmath>

#include <QDebug>

CvmobVideoModel::CvmobVideoModel(QObject *parent) :
    QAbstractItemModel(parent),
    _cvmobVideoData(new QList<CvmobVideoModel::Video>)
{
}

QModelIndex CvmobVideoModel::index(int row, int column, const QModelIndex &parent) const
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
        case 0:
            return createIndex(row, column, new InternalData(DistanceData, row, parentInternalPointer));
        case 1:
            return createIndex(row, column, new InternalData(LinearTrajectoryData, row, parentInternalPointer));
        case 2:
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

QModelIndex CvmobVideoModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return QModelIndex();
    }

    InternalData *childParent = static_cast<InternalData *>(child.internalPointer())->parent;

    if (!childParent) {
        return QModelIndex();
    }

    int parentColumn;

    switch (childParent->type) {
    case DistanceData:
        parentColumn = 0;
        break;
    case LinearTrajectoryData:
        parentColumn = 1;
        break;
    case AngularTrajectoryData:
        parentColumn = 2;
        break;
    case LinearTrajectoryInstantData:
    case AngularTrajectoryInstantData:
        parentColumn = 0;
        break;
    default:
        return QModelIndex();
    }

    return createIndex(childParent->row, parentColumn, childParent);
}

QVariant CvmobVideoModel::data(const QModelIndex &index, int role) const
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
        case 0:
            return currentVideo.fileName;
        case 1:
            return currentVideo.currentFrame;
        case 2:
            return currentVideo.frameCount;
        case 3:
            return currentVideo.frameDuration;
        default:
            return QVariant();
        }
    } else if (internalPointer->type == DistanceData) {
        if (!internalPointer->parent) {
            return QVariant();
        }

        Video currentVideo = _cvmobVideoData->at(internalPointer->parent->row);

        if (index.row() >= currentVideo.distances.size()) {
            return QVariant();
        }

        switch (index.column()) {
        case 0:
            return currentVideo.distances.at(index.row()).first;
        case 1:
            return currentVideo.distances.at(index.row()).second;
        default:
            return QVariant();
        }
    } else {
        InternalData *parentPointer = internalPointer->parent;

        if (!parentPointer &&
            !parentPointer->parent) {
            return QVariant();
        }

        Video currentVideo = _cvmobVideoData->at(parentPointer->parent->row);

        if (internalPointer->type == LinearTrajectoryInstantData) {
            LinearTrajectory currentTrajectory = currentVideo.linearTrajectories.at(parentPointer->row);
            LinearTrajectoryInstant currentInstant = currentTrajectory.instants.at(index.row());

            switch (index.column()) {
            case 0:
                return currentInstant.frame;
            case 1:
                return currentInstant.position;
            case 2:
                return currentInstant.speed;
            case 3:
                return currentInstant.acceleration;
            }
        } else if (internalPointer->type == AngularTrajectoryInstantData) {
            AngularTrajectory currentTrajectory = currentVideo.angularTrajectories.at(parentPointer->row);
            AngularTrajectoryInstant currentInstant = currentTrajectory.instants.at(index.row());

            switch (index.column()) {
            case 0:
                return currentInstant.frame;
            case 1:
                return currentInstant.speed;
            case 2:
                return currentInstant.acceleration;
            case 3:
                return currentInstant.centralEdge;
            case 4:
                return currentInstant.peripheralEdges.first;
            case 5:
                return currentInstant.peripheralEdges.second;
            }
        }
    }

    return QVariant();
}

QVariant CvmobVideoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != VideoSceneRole ||
        role != VideoSceneEditRole ||
        role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("File name");
        case 1:
            return tr("Current frame");
        case 2:
            return tr("Frame count");
        case 3:
            return tr("Frame duration");
        }
    }

    return QVariant();
}

int CvmobVideoModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 4;
    }

    switch (static_cast<InternalData *>(parent.internalPointer())->type) {
    case VideoData:
        switch (parent.column()) {
        case 0:
            return 2;
        case 1:
        case 2:
            return 1;
        default:
            break;
        }
    case LinearTrajectoryData:
        return 4;
    case AngularTrajectoryData:
        return 6;
    default:
        break;
    }

    return 0;
}

int CvmobVideoModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return _cvmobVideoData->size();
    }

    InternalData *internalPointer = static_cast<InternalData *>(parent.internalPointer());
    Video parentVideo = _cvmobVideoData->at(internalPointer->row);

    if (internalPointer->type == VideoData) {
        switch (parent.column()) {
        case 0:
            return parentVideo.distances.size();
        case 1:
            return parentVideo.linearTrajectories.size();
        case 2:
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

Qt::ItemFlags CvmobVideoModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool CvmobVideoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != VideoSceneEditRole ||
        !index.isValid() ||
        !value.isValid()) {
        return false;
    }

    InternalData *internalPointer = static_cast<InternalData *>(index.internalPointer());

    if (internalPointer->type == VideoData) {
        Video &currentVideo = (*_cvmobVideoData)[index.row()];

        switch (index.column()) {
        case 0:
            currentVideo.fileName = value.toString();
            return true;
        case 1:
            currentVideo.currentFrame = value.toInt();
            return true;
        case 2:
            currentVideo.frameCount = value.toInt();
            return true;
        case 3:
            currentVideo.frameDuration = value.toInt();
            return true;
        }
    } else if (internalPointer->type == DistanceData) {
        if (!internalPointer->parent) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[internalPointer->parent->row];

        switch (index.column()) {
        case 0:
            currentVideo.distances[index.row()].first = value.toPointF();
            return true;
        case 1:
            currentVideo.distances[index.row()].second = value.toPointF();
            return true;
        }
    } else {
        InternalData *parentPointer = internalPointer->parent;

        if (!parentPointer &&
            !parentPointer->parent) {
            return false;
        }

        Video &currentVideo = (*_cvmobVideoData)[parentPointer->parent->row];

        if (internalPointer->type == LinearTrajectoryInstantData) {
            LinearTrajectory &currentTrajectory = currentVideo.linearTrajectories[parentPointer->row];
            LinearTrajectoryInstant &currentInstant = currentTrajectory.instants[index.row()];

            switch (index.column()) {
            case 0:
                currentInstant.frame = value.toInt();
                return true;
            case 1:
                currentInstant.position = value.toPointF();
                return true;
            case 2:
                currentInstant.speed = value.toPointF();
                return true;
            case 3:
                currentInstant.acceleration = value.toPointF();
                return true;
            }
        } else if (internalPointer->type == AngularTrajectoryInstantData) {
            AngularTrajectory &currentTrajectory = currentVideo.angularTrajectories[parentPointer->row];
            AngularTrajectoryInstant &currentInstant = currentTrajectory.instants[index.row()];

            switch (index.column()) {
            case 0:
                currentInstant.frame = value.toInt();
                return true;
            case 1:
                currentInstant.speed = value.toInt();
                return true;
            case 2:
                currentInstant.acceleration = value.toInt();
                return true;
            case 3:
                currentInstant.centralEdge = value.toPointF();
                return true;
            case 4:
                currentInstant.peripheralEdges.first = value.toPointF();
                return true;
            case 5:
                currentInstant.peripheralEdges.second = value.toPointF();
                return true;
            }
        }
    }

    return false;
}

template <class T> bool CvmobVideoModel::checkAndInsertRowsIn(QList<T> &l,
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

bool CvmobVideoModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid()) {
        return checkAndInsertRowsIn<Video>(*_cvmobVideoData, row, count);
    }

    InternalData *parentPointer = static_cast<InternalData *>(parent.internalPointer());
    Video &currentVideo = (*_cvmobVideoData)[parentPointer->row];

    if (parentPointer->type == VideoData) {
        switch (parent.column()) {
        case 0:
            return checkAndInsertRowsIn<QPair<QPointF, QPointF> >(currentVideo.distances, row, count, parent);
        case 1:
            return checkAndInsertRowsIn<LinearTrajectory>(currentVideo.linearTrajectories, row, count, parent);
        case 2:
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

template <class T> bool CvmobVideoModel::checkAndRemoveRowsFrom(QList<T> &l,
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

    endInsertRows();

    return true;
}

bool CvmobVideoModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid()) {
        return checkAndRemoveRowsFrom<Video>(*_cvmobVideoData, row, count);
    }

    InternalData *parentPointer = static_cast<InternalData *>(parent.internalPointer());
    Video &currentVideo = (*_cvmobVideoData)[parentPointer->row];

    if (parentPointer->type == VideoData) {
        switch (parent.column()) {
        case 0:
            return checkAndRemoveRowsFrom<QPair<QPointF, QPointF> >(currentVideo.distances, row, count, parent);
        case 1:
            return checkAndRemoveRowsFrom<LinearTrajectory>(currentVideo.linearTrajectories, row, count, parent);
        case 2:
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

typedef QPair<QPointF, QPointF> QPointFPair;
void CvmobVideoModel::qDebugAllData() const
{
    foreach (Video v, *_cvmobVideoData) {
        qDebug() << "---";
        qDebug() << v.fileName.toUtf8().constData();
        qDebug() << QString("%1 frames (%2ms each)").arg(v.frameCount).arg(v.frameDuration).toUtf8().constData();
        qDebug() << "Distances measured:";
        foreach (QPointFPair dist, v.distances) {
            qDebug() << QString("- %1 pixels (from point (%2, %3) to point (%4, %5))")
                        .arg(sqrt(pow(dist.first.x() - dist.second.x(), 2) +
                                  pow(dist.first.y() - dist.second.y(), 2)))
                        .arg(dist.first.x()).arg(dist.first.y())
                        .arg(dist.second.y()).arg(dist.second.y());
        }
    }
}
