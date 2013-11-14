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

#include <model/jobs/anglecalcjob.hpp>
#include <model/jobs/trajectorycalcjob.hpp>

class JobHandler;

float angleFromPoints(const QPointF& c, const QPointF& e1, const QPointF& e2);

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
        AngleColumn,
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
    ~VideoModel();

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
    
    bool openVideo(const QString &path);
    JobHandler *jobHandlerForVideo(int videoRow);
    void createDistance(const QLineF& l, const QModelIndex &videoIndex);
    void createDistance(const QLineF &l, int videoRow);
    void createDistance(const QPointF& p1, const QPointF& p2, const QModelIndex& videoIndex);
    void createDistance(const QPointF& p1, const QPointF& p2, int videoRow);
    AngleCalcJob *calculateAngle(const QVector<QPointF> &anglePoints, int frame, int videoRow,
                                 const QSize &windowSize = QSize(21, 21),
                                 CalculationFlags flags = FromHereOnwards);
    TrajectoryCalcJob *calculateTrajectory(const QPointF &p, int frame,
                                                       int videoRow,
                                                       const QSize &windowSize = QSize(21, 21),
                                                       CalculationFlags flags = FromHereOnwards);

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
        int frame;
        float speed, acceleration;
        QPointF centralEdge;
        QPair<QPointF, QPointF> peripheralEdges;
    };

    struct Angle
    {
        QList<AngleInstant> instants;
    };

    struct Video
    {
        Video() :
            streamFrame(-1),
            jobHandler(0)
        {}
        ~Video();
        
        QString fileName;
        int currentFrame, frameDuration, frameCount, streamFrame;
        QImage frameImage;
        QSizeF frameSize;
        cv::VideoCapture videoStream;
        JobHandler *jobHandler;

        QList<QLineF> distances;
        QList<Trajectory> trajectories;
        QList<Angle> angles;
    };

    struct InternalData
    {
        InternalData *parent;
        int row, column;
        QHash<QPair<int, int>, InternalData *> children;
        
        InternalData(int row, int column, InternalData *parent = 0) :
            row(row),
            column(column),
            parent(parent)
        {}
    };
    
    QHash<QPair<int, int>, InternalData *> *_indexesData;

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
