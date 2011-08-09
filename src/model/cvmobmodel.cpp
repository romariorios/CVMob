#include "cvmobmodel.hpp"

#include <cmath>

CvmobModel::CvmobModel(QObject *parent) :
    QAbstractItemModel(parent),
    _cvmobData(new QList<CvmobModel::Video>)
{
}

QModelIndex CvmobModel::index(int row, int column, const QModelIndex &parent) const
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
        case 4:
            return createIndex(row, column, new InternalData(DistanceData, row, parentInternalPointer));
        case 5:
            return createIndex(row, column, new InternalData(LinearTrajectoryData, row, parentInternalPointer));
        case 6:
            return createIndex(row, column, new InternalData(AngularTrajectoryData, row, parentInternalPointer));
        }
    case LinearTrajectoryData:
        if (parentColumn == 0) {
            return createIndex(row, column, new InternalData(LinearTrajectoryInstantData, row, parentInternalPointer));
        }
    case AngularTrajectoryData:
        if (parentColumn == 0) {
            return createIndex(row, column, new InternalData(AngularTrajectoryInstantData, row, parentInternalPointer));
        }
    }

    return QModelIndex();
}

QModelIndex CvmobModel::parent(const QModelIndex &child) const
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
        parentColumn = 4;
        break;
    case LinearTrajectoryData:
        parentColumn = 5;
        break;
    case AngularTrajectoryData:
        parentColumn = 6;
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

QVariant CvmobModel::data(const QModelIndex &index, int role) const
{
    if (role != VideoSceneRole ||
        role != VideoSceneEditRole ||
        role != Qt::DisplayRole ||
        !index.isValid()) {
        return QVariant();
    }

    InternalData *internalPointer = static_cast<InternalData *>(index.internalPointer());

    if (internalPointer->type == VideoData) {
        Video currentVideo = _cvmobData->at(index.row());

        switch (index.column()) {
        case 0:
            return currentVideo.fileName;
        case 1:
            return currentVideo.currentFrame;
        case 2:
            return currentVideo.frameCount;
        case 3:
            return currentVideo.frameDuration;
        }
    } else if (internalPointer->type == DistanceData) {
        if (!internalPointer->parent) {
            return QVariant();
        }

        Video currentVideo = _cvmobData->at(internalPointer->parent->row);

        switch (index.column()) {
        case 0:
            return currentVideo.distances.at(index.row()).first;
        case 1:
            return currentVideo.distances.at(index.row()).second;
        }
    } else {
        InternalData *parentPointer = internalPointer->parent;

        if (!parentPointer &&
            !parentPointer->parent) {
            return QVariant();
        }

        Video currentVideo = _cvmobData->at(parentPointer->parent->row);

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

QVariant CvmobModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int CvmobModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return 7;
    }

    switch (static_cast<InternalData *>(parent.internalPointer())->type) {
    case VideoData:
        switch (parent.column()) {
        case 4:
            return 2;
        case 5:
        case 6:
            return 1;
        }
    case LinearTrajectoryData:
        return 4;
    case AngularTrajectoryData:
        return 6;
    }

    return 0;
}

int CvmobModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return _cvmobData->size();
    }

    InternalData *internalPointer = static_cast<InternalData *>(parent.internalPointer());
    Video parentVideo = _cvmobData->at(internalPointer->row);

    if (internalPointer->type == VideoData) {
        switch (parent.column()) {
        case 4:
            return parentVideo.distances.size();
        case 5:
            return parentVideo.linearTrajectories.size();
        case 6:
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
