/*
 * IExportStrategy.cpp
 *
 *  Created on: 27/09/2008
 *      Author: paulista
 */

#include "IExportStrategy.h"

namespace model {

IExportStrategy::IExportStrategy() {}

void IExportStrategy::setFile(char* filename){}

void IExportStrategy::exportData(int count,  QVector <double> time, QVector <double> velocity, QVector <double> acceleration, QVector <double> work,
                                 QVector <CvPoint2D32f> trajectorie,
                                 double horizontalRazao, double verticalRazao,int initFrame,
                                 QVector <double> velocityX,QVector <double> velocityY,QVector <double> accelerationX,QVector <double> accelerationY){}
void IExportStrategy::exportReport(int count,QVector <double> time, QVector <double> velocity, QVector <double> acceleration, QVector <double> work,
                                 QVector <CvPoint2D32f> trajectorie,
                                 double horizontalRazao, double verticalRazao,int initFrame,
                                 QVector <double> velocityX,QVector <double> velocityY,QVector <double> accelerationX,QVector <double> accelerationY){}
void IExportStrategy::closeFile(){}

IExportStrategy::~IExportStrategy() {}

}
