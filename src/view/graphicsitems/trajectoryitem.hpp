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

#ifndef TRAJECTORYITEM_HPP
#define TRAJECTORYITEM_HPP

#include <QGraphicsItemGroup>

#include <QList>
#include <QPair>

class QGraphicsItemGroup;
class QGraphicsLineItem;
class TrajectoryInstantItem;

class TrajectoryItem : public QGraphicsItemGroup
{
public:
    TrajectoryItem(const QColor &color, const QSizeF &videoSize, QGraphicsItem *parent = 0);
    ~TrajectoryItem();

    enum DrawingPolicy
    {
        NoDraw = 0x1,
        DrawBefore = 0x2,
        DrawBeforeAndAfter = 0x4
    };
    DrawingPolicy drawTrajectory() const;
    void setDrawTrajectory(DrawingPolicy policy);

    enum ShowPolicy
    {
        NoShow = 0x1,
        ShowInCurrentInstant = 0x2,
        ShowInEveryInstant = 0x4
    };
    ShowPolicy showSpeed() const;
    void setShowSpeed(ShowPolicy policy);

    ShowPolicy showAccel() const;
    void setShowAccel(ShowPolicy policy);

    int startingFrame() const;
    void setStartingFrame(int frame);

    void setCurrentFrame(int frame);

    void appendInstant(QPointF pos = QPointF(0, 0),
                       QPointF speed = QPointF(0, 0),
                       QPointF accel = QPointF(0, 0));

    TrajectoryInstantItem *instantAt(int pos) const;

    inline int size() const { return _instants.size(); }

private:
    void followDrawPolicy();
    void followShowSpeedPolicy();
    void followShowAccelPolicy();

    DrawingPolicy _drawTrajectory;
    ShowPolicy _showSpeed;
    ShowPolicy _showAccel;
    int _startingFrame;
    int _currentFrame;

    QList<TrajectoryInstantItem *> _instants;

    QGraphicsItem *_currentInstant;
    QColor _color;
    double _instantsScaleFactor;
};

#endif // TRAJECTORYITEM_HPP
