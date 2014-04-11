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

#ifndef PLOTPROXIES_H
#define PLOTPROXIES_H

#include <model/proxies/instantsproxymodel.hpp>

class BasePlotProxyModel : public InstantsProxyModel
{
    Q_OBJECT

public:
    explicit BasePlotProxyModel(QObject* parent = 0);
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    int columnCount(const QModelIndex &p = QModelIndex()) const;

protected:
    virtual QVariant xData(const QModelIndex &index) const { return index.data(); }
    virtual QVariant yData(const QModelIndex &index) const { return index.data(); }
    virtual QString graphTitle() const = 0;
    virtual QString xTitle() const { return tr("Frame"); }
    virtual QString yTitle() const = 0;
    virtual int xColumn() const = 0;
    virtual int yColumn() const = 0;
};

class XTrajectoryPlotProxyModel : public BasePlotProxyModel
{
    Q_OBJECT

public:
    inline explicit XTrajectoryPlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent)
    {
        setColumn(VideoModel::AllTrajectoriesCol);
    }

protected:
    inline QVariant yData(const QModelIndex& index) const { return index.data().toPointF().x(); }
    inline QString graphTitle() const { return tr("X (Trajectory)"); }
    inline QString yTitle() const { return tr("X"); }
    inline int xColumn() const { return VideoModel::LFrameCol; }
    inline int yColumn() const { return VideoModel::PositionCol; }
};

class YTrajectoryPlotProxyModel : public BasePlotProxyModel
{
    Q_OBJECT

public:
    inline explicit YTrajectoryPlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent)
    {
        setColumn(VideoModel::AllTrajectoriesCol);
    }

protected:
    inline QVariant yData(const QModelIndex& index) const { return index.data().toPointF().y(); }
    inline QString graphTitle() const { return tr("Y (Trajectory)"); }
    inline QString yTitle() const { return tr("Y"); }
    inline int xColumn() const { return VideoModel::LFrameCol; }
    inline int yColumn() const { return VideoModel::PositionCol; }
};

class TrajectorySpeedPlotProxyModel : public BasePlotProxyModel
{
public:
    inline explicit TrajectorySpeedPlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent)
    {
        setColumn(VideoModel::AllTrajectoriesCol);
    }

protected:
    inline QVariant yData(const QModelIndex& index) const { return QLineF(QPointF(0, 0), index.data().toPointF()).length(); }
    inline QString graphTitle() const { return tr("Speed"); }
    inline QString yTitle() const { return tr("Speed (pxl/frame)"); }
    inline int xColumn() const { return VideoModel::LFrameCol; }
    inline int yColumn() const { return VideoModel::LSpeedCol; }
};

class TrajectoryAccelPlotProxyModel : public BasePlotProxyModel
{
public:
    inline explicit TrajectoryAccelPlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent)
    {
        setColumn(VideoModel::AllTrajectoriesCol);
    }

protected:
    inline QVariant yData(const QModelIndex& index) const { return QLineF(QPointF(0, 0), index.data().toPointF()).length(); }
    inline QString graphTitle() const { return tr("Acceleration"); }
    inline QString yTitle() const { return tr("Acceleration (pxl/frame^2)"); }
    inline int xColumn() const { return VideoModel::LFrameCol; }
    inline int yColumn() const { return VideoModel::LAccelCol; }
};

class AnglePlotProxyModel : public BasePlotProxyModel
{
public:
    inline explicit AnglePlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent)
    {
        setColumn(VideoModel::AllAnglesCol);
    }

protected:
    inline QString graphTitle() const { return tr("Angle"); }
    inline QString yTitle() const { return tr("Angle (radians)"); }
    inline int xColumn() const { return VideoModel::AFrameCol; }
    inline int yColumn() const { return VideoModel::AngleCol; }
};

#endif // PLOTPROXIES_H
