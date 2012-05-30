#ifndef CVMOBMODEL_HPP
#define CVMOBMODEL_HPP

#include <QAbstractItemModel>

#include <QList>
#include <QPair>
#include <QPointF>

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
        int currentFrame, frameCount, frameDuration;

        QList<QPair<QPointF, QPointF> > distances;
        QList<LinearTrajectory> linearTrajectories;
        QList<AngularTrajectory> angularTrajectories;
    };

    enum InternalDataType
    {
        VideoData,
        DistanceData,
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

    void qDebugDataInRow(int row) const;
    void qDebugAllData() const;
};

#endif // CVMOBMODEL_HPP
