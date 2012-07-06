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

#ifndef CVMOBMODEL_HPP
#define CVMOBMODEL_HPP

#include <QAbstractItemModel>

#include <QImage>
#include <QList>
#include <QLineF>
#include <QPointF>
#include <QSizeF>

class CvmobVideoModel : public QAbstractItemModel
{
    Q_OBJECT

    struct LinearTrajectoryInstant
    {
        int frame;
        QPointF position, speed, acceleration;
    };

    struct LinearTrajectory
    {
        QList<LinearTrajectoryInstant> instants;
    };

    struct AngularTrajectoryInstant
    {
        int frame, speed, acceleration;
        QPointF centralEdge;
        QPair<QPointF, QPointF> peripheralEdges;
    };

    struct AngularTrajectory
    {
        QList<AngularTrajectoryInstant> instants;
    };

    struct Video
    {
        QString fileName;
        int currentFrame, frameDuration;
        QSizeF frameSize;

        QList<QImage> frames;
        QList<QLineF> distances;
        QList<LinearTrajectory> linearTrajectories;
        QList<AngularTrajectory> angularTrajectories;
    };

    enum InternalDataType
    {
        VideoData,
        DistanceData,
        FrameData,
        LinearTrajectoryData,
        AngularTrajectoryData,
        LinearTrajectoryInstantData,
        AngularTrajectoryInstantData
    };

    struct InternalData
    {
        InternalDataType type;
        int row;
        InternalData *parent;

        InternalData(InternalDataType e_type, int e_row, InternalData *e_parent = 0) :
            type(e_type),
            row(e_row),
            parent(e_parent) {}
    };

    QList<Video> *_cvmobVideoData;

    double calculateDistance(long row) const;
    template <class T> bool checkAndInsertRowsIn(QList<T> &l,
                                                 int row,
                                                 int count,
                                                 const QModelIndex &parent = QModelIndex());
    template <class T> bool checkAndRemoveRowsFrom(QList<T> &l,
                                                   int row,
                                                   int count,
                                                   const QModelIndex &parent = QModelIndex());

public:
    enum VideoColumn {
        FileNameColumn = 0,
        CurrentFrameColumn,
        FrameDurationColumn,
        FrameSizeColumn,
        FramesColumn,
        DistancesColumn,
        LinearTrajectoriesColumn,
        AngularTrajectoriesColumn,
        VideoColumnCount
    };

    enum LinearTrajectoryInstantColumn {
        LFrameColumn = 0,
        PositionColumn,
        LSpeedColumn,
        LAccelerationColumn,
        LinearTrajectoryInstantColumnCount
    };

    enum AngularTrajectoryInstantColumn {
        AFrameColumn = 0,
        ASpeedColumn,
        AAccelerationColumn,
        CentralEdgeColumn,
        PeripheralEdge1Column,
        PeripheralEdge2Column,
        AngularTrajectoryInstantColumnCount
    };

    explicit CvmobVideoModel(QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    enum GraphicsViewRole
    {
        VideoSceneRole,
        VideoSceneEditRole,
        VideoListRole
    };
};

#endif // CVMOBMODEL_HPP
