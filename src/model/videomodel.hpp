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
#include <QMutex>
#include <QSizeF>

#include <opencv/highgui.h>
#include <model/trajectorycalcjob.hpp>

class VideoModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum VideoColumn {
        InvalidColumn = -1,
        FileNameColumn = 0,
        CurrentFrameColumn,
        FrameDurationColumn,
        FrameSizeColumn,
        FramesColumn,
        DistancesColumn,
        TrajectoriesColumn,
        AnglesColumn,
        VideoColumnCount
    };

    enum TrajectoryInstantColumn {
        LFrameColumn = 0,
        PositionColumn,
        LSpeedColumn,
        LAccelerationColumn,
        TrajectoryInstantColumnCount
    };

    enum AngleInstantColumn {
        AFrameColumn = 0,
        ASpeedColumn,
        AAccelerationColumn,
        CentralEdgeColumn,
        PeripheralEdge1Column,
        PeripheralEdge2Column,
        AngleInstantColumnCount
    };

    enum CalculationFlags {
        FromHereOnwards = 0x1,
        FromHereBackwards = 0x2
    };

    explicit VideoModel(QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = VideoSceneRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = VideoSceneEditRole);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    
    bool openVideo(const QString &path);
    void createDistance(const QLineF& l, const QModelIndex &videoIndex);
    void createDistance(const QLineF &l, int videoRow);
    void createDistance(const QPointF& p1, const QPointF& p2, const QModelIndex& videoIndex);
    void createDistance(const QPointF& p1, const QPointF& p2, int videoRow);
    TrajectoryCalcJob *calculateTrajectory(const QPointF &p, int frame,
                                                       int videoRow,
                                                       const QSize &windowSize = QSize(21, 21),
                                                       CalculationFlags flags = FromHereOnwards);

    enum GraphicsViewRole
    {
        VideoSceneRole,
        VideoSceneEditRole,
        VideoListRole
    };

private:
    struct TrajectoryInstant
    {
        int frame;
        QPointF position, speed, acceleration;
    };

    struct Trajectory
    {
        QList<TrajectoryInstant> instants;
    };

    struct AngleInstant
    {
        int frame, speed, acceleration;
        QPointF centralEdge;
        QPair<QPointF, QPointF> peripheralEdges;
    };

    struct Angle
    {
        QList<AngleInstant> instants;
    };

    struct Video
    {
        QString fileName;
        int currentFrame, frameDuration, frameCount;
        QSizeF frameSize;
        cv::VideoCapture videoStream;

        QList<QLineF> distances;
        QList<Trajectory> trajectories;
        QList<Angle> angles;
    };

    enum InternalDataType
    {
        VideoData,
        DistanceData,
        FrameData,
        TrajectoryData,
        AngleData,
        TrajectoryInstantData,
        AngleInstantData
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
    mutable QMutex _streamLock;

    double calculateDistance(long row) const;
    template <class T> bool checkAndInsertRowsIn(QList<T> &l,
                                                 int row,
                                                 int count,
                                                 const QModelIndex &parent = QModelIndex());
    template <class T> bool checkAndRemoveRowsFrom(QList<T> &l,
                                                   int row,
                                                   int count,
                                                   const QModelIndex &parent = QModelIndex());
};

#endif // CVMOBMODEL_HPP
