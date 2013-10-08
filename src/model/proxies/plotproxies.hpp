/*
    CVMob - Motion capture program
    Copyright (C) 2013  The CVMob contributors

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

#ifndef PLOTPROXIES_H
#define PLOTPROXIES_H

#include <model/proxies/instantsproxymodel.hpp>

#include <model/proxies/baseproxymodel.hpp>

class BasePlotProxyModel : public InstantsProxyModel
{
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
    virtual QString xTitle() const { return tr("Time (pxl)"); }
    virtual QString yTitle() const = 0;
    virtual int xColumn() const = 0;
    virtual int yColumn() const = 0;
};

class XTrajectoryPlotProxyModel : public BasePlotProxyModel
{
public:
    inline explicit XTrajectoryPlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent) {}
    
protected:
    inline QVariant yData(const QModelIndex& index) const { return index.data().toPointF().x(); }
    inline QString graphTitle() const { return tr("X (Trajectory)"); }
    inline QString yTitle() const { return tr("X"); }
    inline int xColumn() const { return VideoModel::LFrameColumn; }
    inline int yColumn() const { return VideoModel::PositionColumn; }
};

class YTrajectoryPlotProxyModel : public BasePlotProxyModel
{
public:
    inline explicit YTrajectoryPlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent) {}
    
protected:
    inline QVariant yData(const QModelIndex& index) const { return index.data().toPointF().y(); }
    inline QString graphTitle() const { return tr("Y (Trajectory)"); }
    inline QString yTitle() const { return tr("Y"); }
    inline int xColumn() const { return VideoModel::LFrameColumn; }
    inline int yColumn() const { return VideoModel::PositionColumn; }
};

class TrajectorySpeedPlotProxyModel : public BasePlotProxyModel
{
public:
    inline explicit TrajectorySpeedPlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent) {}
    
protected:
    inline QVariant yData(const QModelIndex& index) const { return QLineF(QPointF(0, 0), index.data().toPointF()).length(); }
    inline QString graphTitle() const { return tr("Speed"); }
    inline QString yTitle() const { return tr("Speed (pxl/frame)"); }
    inline int xColumn() const { return VideoModel::LFrameColumn; }
    inline int yColumn() const { return VideoModel::LSpeedColumn; }
};

class TrajectoryAccelPlotProxyModel : public BasePlotProxyModel
{
public:
    inline explicit TrajectoryAccelPlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent) {}
    
protected:
    inline QVariant yData(const QModelIndex& index) const { return QLineF(QPointF(0, 0), index.data().toPointF()).length(); }
    inline QString graphTitle() const { return tr("Acceleration"); }
    inline QString yTitle() const { return tr("Acceleration (pxl/frame^2)"); }
    inline int xColumn() const { return VideoModel::LFrameColumn; }
    inline int yColumn() const { return VideoModel::LAccelerationColumn; }
};

class AnglePlotProxyModel : public BasePlotProxyModel
{
public:
    inline explicit AnglePlotProxyModel(QObject *parent = 0) : BasePlotProxyModel(parent) {}
    
protected:
    inline QString graphTitle() const { return tr("Angle"); }
    inline QString yTitle() const { return tr("Angle (radians)"); }
    inline int xColumn() const { return VideoModel::AFrameColumn; }
    inline int yColumn() const { return VideoModel::AngleColumn; }
};

#endif // PLOTPROXIES_H
