/*
 * Point.h
 *
 *  Created on: 01/09/2008
 *      Author: paulista
 */

#ifndef POINT_H_
#define POINT_H_

#define MAX_FRAMES 10000

#include <opencv/cv.h>
#include <QVector>
using namespace std;
using namespace cv;

struct point
{
	int index;
	int color[3];
        char type;          // 0 = Trajectory,   1 = angle vertice.
	CvPoint2D32f markedPoint;
	int countTrajectorie;
        QVector <CvPoint2D32f> trajectorie;
        QVector <double> velocity;
        QVector <double> xVelocity;
        QVector <double> yVelocity;
        QVector <double> xAcceleration;
        QVector <double> yAcceleration;
        QVector <double> time;
        QVector <double> acceleration;
        QVector <double> work;
        QVector <double> traj_x;
        QVector <double> traj_y;
        Point2f lastPoint;
        int initFrame;   // initial captured frame (the frame of the first mouse clicked on video)

};

struct FixedPoint
{
        int index;
        int color[3];
        CvPoint2D32f markedPoint;
        CvPoint2D32f features[1];
        CvPoint2D32f prev_features[1];
        int initFrame;   // initial captured frame (the frame of the first mouse clicked on video)

};
#endif /* POINT_H_ */

