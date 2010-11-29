/*
 * IExportStrategy.h
 *
 *  Created on: 27/09/2008
 *      Author: paulista
 */

#ifndef IEXPORTSTRATEGY_H_
#define IEXPORTSTRATEGY_H_
#include <opencv/cv.h>
#include <QVector>
using namespace std;
namespace model {

class IExportStrategy {
public:
	IExportStrategy();
	virtual void setFile(char* filename);
        virtual void exportData(int count, QVector <double> time, QVector <double> velocity, QVector <double> acceleration,
                                QVector <double> work, QVector <CvPoint2D32f> trajectorie, double horizontalRazao,
                                double verticalRazao,int initFrame,QVector <double> velocityX,QVector <double> velocityY,
                                QVector <double> accelerationX,QVector <double> accelerationY);
        virtual void exportReport(int count, QVector <double> time, QVector <double> velocity, QVector <double> acceleration,
                                QVector <double> work, QVector <CvPoint2D32f> trajectorie, double horizontalRazao,
                                double verticalRazao,int initFrame,QVector <double> velocityX,QVector <double> velocityY,
                                QVector <double> accelerationX,QVector <double> accelerationY);
	virtual void closeFile();
	virtual ~IExportStrategy();
};

}

#endif /* IEXPORTSTRATEGY_H_ */
