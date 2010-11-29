/*
 * Angle.h
 *
 *  Created on: 10/03/2010
 *      Author: Garcia
 */

#ifndef ANGLE_H_
#define ANGLE_H_


#include <opencv/cv.h>
#include <QVector>

class Angle
{
public:
	int index;
	int color[3];
        CvPoint2D32f markedPoint;
        QVector <CvPoint2D32f> vertices;  // maximum of 3
        int countVertices;
        double ang1,ang2;               // Angles for drawing
        QVector <double> value;
        QVector <double> velocity;
        QVector <double> time;
        QVector <double> acceleration;
        int initFrame;   // initial captured frame (the frame of the first mouse clicked on video)
        void calcValue(double time);

};

#endif /* ANGLE_H_ */

