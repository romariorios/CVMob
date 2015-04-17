/*
    CVMob - Motion capture program
    Copyright (C) 2013--2015  The CVMob contributors

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

#include <QColor>
#include <QImage>
#include <QList>
#include <QLineF>
#include <QPointF>
#include <QMutex>
#include <QSizeF>

#include <memory>
#include <vector>

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
        InvalidCol = -1,
        FileNameCol = 0,
        CurrentFrameCol,
        FrameDurationCol,
        FrameSizeCol,
        AllFramesCol,
        PlayStatusCol,
        CalibrationRatioCol,
        OriginPointCol,
        AllDistancesCol,
        AllTrajectoriesCol,
        AllAnglesCol,
        VideoColCount
    };

    enum TrajectoryColumn {
        AllTrajectoryInstantsCol = 0,
        TrajectoryColorCol,
        TrajectoryColCount
    };

    enum AngleColumn {
        AllAngleInstantsCol = 0,
        AngleColorCol,
        AngleColCount
    };

    enum TrajectoryInstantColumn {
        LFrameCol = 0,
        PositionCol,
        LSpeedCol,
        LAccelCol,
        TrajectoryInstantColCount
    };

    enum AngleInstantColumn {
        AFrameCol = 0,
        AngleCol,
        ASpeedCol,
        AAccelCol,
        ACenterCol,
        AEdge1Col,
        AEdge2Col,
        AngleInstantColCount
    };

    enum CalculationFlags {
        FromHereOnwards = 0x1,
        FromHereBackwards = 0x2
    };

    enum {
        VideoDataRole = Qt::UserRole + 2
    };

    struct IndexDir
    {
        IndexDir(int row, int column) :
            row { row },
            column { column }
        {}

        IndexDir(const QModelIndex &ind) :
            IndexDir { ind.row(), ind.column() }
        {}

        IndexDir() = default;

        int row = -1;
        int column = -1;
    };

    typedef QVector<IndexDir> IndexPath;

    explicit VideoModel(QObject *parent = 0);
    ~VideoModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(const QModelIndex &parent, const IndexPath &path) const;
    QModelIndex index(const IndexPath &path) const;
    QModelIndex parent(const QModelIndex &child) const;
    static const IndexPath indexPath(const QModelIndex &index);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    int openVideo(const QString &path);
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

    std::shared_ptr<JobHandler> jobHandlerForVideo(int videoRow) const;

public slots:
    void updateSettings();

private:
    struct TrajectoryInstant
    {
        int frame;
        QPointF position, speed, acceleration;
    };

    struct Trajectory
    {
        Trajectory() :
            color { rand() % 255, rand() % 255, rand() % 255 }
        {}

        QColor color;
        std::vector<TrajectoryInstant> instants;
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
        Angle() :
            color { rand() % 255, rand() % 255, rand() % 255 }
        {}

        QColor color;
        std::vector<AngleInstant> instants;
    };

    struct Video
    {
        Video() :
            streamFrame(-1),
            playStatus(false)
        {}

        inline void setCalibrationRatio(double ratioValue)
        {
            if (!calibration) {
                calibration = new double;
            }

            *calibration = ratioValue;
        }

        inline void unsetCalibration()
        {
            delete calibration;
            calibration = nullptr;
        }

        inline double calibrationRatio() const
        {
            return calibration? *calibration : 1;
        }

        inline QPointF calibratedPoint(const QPointF &p) const
        {
            return (p - originPoint) * calibrationRatio();
        }

        QString fileName;
        int currentFrame, frameDuration, frameCount, streamFrame;
        double *calibration = nullptr;
        QPointF originPoint = {0, 0};
        bool playStatus;
        QImage frameImage;
        QSizeF frameSize;
        cv::VideoCapture videoStream;
        std::shared_ptr<JobHandler> jobHandler{nullptr};

        std::vector<QLineF> distances;
        std::vector<Trajectory> trajectories;
        std::vector<Angle> angles;
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

    typedef QList<IndexDir> IndexPathList;

    QHash<QPair<int, int>, InternalData *> *_indexesData;

    std::vector<Video> *_cvmobVideoData = new std::vector<Video>;
    mutable QMutex _streamLock;

    double calculateDistance(long row) const;

    template <class T>
    bool checkAndInsertRowsIn(
        std::vector<T> &l,
        int row,
        int count,
        const QModelIndex &parent = {});

    template <class T>
    bool checkAndRemoveRowsFrom(
        std::vector<T> &l,
        int row,
        int count,
        const QModelIndex &parent = {});
};

#endif // CVMOBMODEL_HPP
