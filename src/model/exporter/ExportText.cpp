/*
 * ExportText.cpp
 *
 *  Created on: 27/09/2008
 *      Author: paulista
 */

#include "ExportText.h"
#include<iostream>

namespace model{

ExportText::ExportText() {
	// TODO Auto-generated constructor stub

}

void ExportText::setFile(char* filename){
	file.open(filename);
}

void ExportText::exportData(int count, QVector <double> time, QVector <double> velocity, QVector <double> acceleration,
                            QVector <double> work, QVector <CvPoint2D32f> trajectorie, double horizontalRazao,
                            double verticalRazao,int initFrame,QVector <double> velocityX,QVector <double> velocityY,
                            QVector <double> accelerationX,QVector <double> accelerationY){
        double aux;


        file << "TIME" << "\t" << "VELOCITY" << "\t"<< "VELOCITY_X" << "\t"<< "VELOCITY_Y" << "\t" <<
                "ACCELERATION" << "\t" << "Acceler_X" << "\t"<< "Acceler_Y" << "\t"<<  "WORK" << "\t" <<
                "X" << "\t" << "Y" << endl;
        for(int i =0; i < count-1; i++){
                aux=accelerationX[i];
                file << time[i] << "\t" << velocity[i] << "\t"<< velocityX[i] << "\t" << velocityY[i] << "\t"<<
                        acceleration[i] << "\t" << accelerationX[i] << "\t" << accelerationY[i] << "\t"<<
                         work[i] << "\t" << trajectorie[i].x*horizontalRazao << "\t" << trajectorie[i].y*verticalRazao << endl;
        }
	file << endl;
}

void ExportText::exportReport(int count, QVector <double> time, QVector <double> velocity, QVector <double> acceleration,
                              QVector <double> work, QVector <CvPoint2D32f> trajectorie, double horizontalRazao,
                              double verticalRazao,int initFrame,QVector <double> velocityX,QVector <double> velocityY,
                              QVector <double> accelerationX,QVector <double> accelerationY)
{
    double vel[4],velX[4],velY[4],acel[4],acelX[4],acelY[4],x[4],y[4];
    double traject=0.0;
    int i,N=0;
    QString line;


    for(i = 0;i<4;i++)
        vel[i]=velX[i]=velY[i]=acel[i]=acelX[i]=acelY[i]=x[i]=y[i]=0;
    vel[0]=velX[0]=velY[0]=acel[0]=acelX[0]=acelY[0]=x[0]=y[0]=-99999999;   // Min
    vel[1]=velX[1]=velY[1]=acel[1]=acelX[1]=acelY[1]=x[1]=y[1]=99999999;    // Max
    //Calculates the Maximum, Minimum and the average value
    for( i =0; i < count; i++){
        // Velocity
        vel[0]=MAX(vel[0],velocity[i]);
        vel[1]=MIN(vel[1],velocity[i]);
        vel[2]+=velocity[i];

        velX[0]=MAX(velX[0],velocityX[i]);
        velX[1]=MIN(velX[1],velocityX[i]);
        velX[2]+=velocityX[i];

        velY[0]=MAX(velY[0],velocityY[i]);
        velY[1]=MIN(velY[1],velocityY[i]);
        velY[2]+=velocityY[i];
        // Acceleration
        acel[0]=MAX(acel[0],acceleration[i]);
        acel[1]=MIN(acel[1],acceleration[i]);
        acel[2]+=acceleration[i];

        acelX[0]=MAX(acelX[0],accelerationX[i]);
        acelX[1]=MIN(acelX[1],accelerationX[i]);
        acelX[2]+=accelerationX[i];

        acelY[0]=MAX(acelY[0],accelerationY[i]);
        acelY[1]=MIN(acelY[1],accelerationY[i]);
        acelY[2]+=accelerationY[i];
        // Location
        x[0]=MAX(x[0],trajectorie[i].x*horizontalRazao);
        x[1]=MIN(x[1],trajectorie[i].x*horizontalRazao);
        x[2]+=trajectorie[i].x*horizontalRazao;

        y[0]=MAX(y[0],trajectorie[i].y*verticalRazao);
        y[1]=MIN(y[1],trajectorie[i].y*verticalRazao);
        y[2]+=trajectorie[i].y*verticalRazao;
        // Trajectorie
        if(i<count-2)
        {
            double dx=(trajectorie[i+1].x-trajectorie[i].x)*horizontalRazao;
            double dy=(trajectorie[i+1].y-trajectorie[i].y)*verticalRazao;
            traject+=sqrt(dx*dx+dy*dy);
        }
        N++;
    }
    // Calculates de standard deviation
    for( i =0; i < count-1; i++){
        vel[3]+=(velocity[i]-vel[2]/N)*(velocity[i]-vel[2]/N);
        velX[3]+=(velocityX[i]-velX[2]/N)*(velocityX[i]-velX[2]/N);
        velY[3]+=(velocityY[i]-velY[2]/N)*(velocityY[i]-velY[2]/N);

        acel[3]+=(acceleration[i]-acel[2]/N)*(acceleration[i]-vel[2]/N);
        acelX[3]+=(accelerationX[i]-acelX[2]/N)*(accelerationX[i]-acelX[2]/N);
        acelY[3]+=(accelerationY[i]-acelY[2]/N)*(accelerationY[i]-acelY[2]/N);

        x[3]+=(trajectorie[i].x*horizontalRazao-x[2]/N)*(trajectorie[i].x*horizontalRazao-x[2]/N);
        y[3]+=(trajectorie[i].y*verticalRazao-y[2]/N)*(trajectorie[i].y*verticalRazao-y[2]/N);
    }
    vel[2]/=N;
    velX[2]/=N;
    velY[2]/=N;
    acel[2]/=N;
    acelX[2]/=N;
    acelY[2]/=N;
    x[2]/=N;
    y[2]/=N;

    vel[3]=sqrt(vel[3]/(N-1));
    velX[3]=sqrt(velX[3]/(N-1));
    velY[3]=sqrt(velY[3]/(N-1));
    acel[3]=sqrt(acel[3]/(N-1));
    acelX[3]=sqrt(acelX[3]/(N-1));
    acelY[3]=sqrt(acelY[3]/(N-1));
    x[3]=sqrt(x[3]/(N-1));
    y[3]=sqrt(y[3]/(N-1));


    line = "Variable\tMED\tDESV\tMAX\tMIN\tMax-Min";
    repo.addText(line);

    makeLine("Velo\t",vel);
    makeLine("VeloX\t",velX);
    makeLine("VeloY\t",velY);
    makeLine("Accel\t",acel);
    makeLine("AcceX\t",acelX);
    makeLine("AcceY\t",acelY);
    makeLine("X    \t",x);
    makeLine("Y    \t",y);


    line = "Trajectory:" + QString::number(traject);
    repo.addText(line);

    line = "Initial Time:" + QString::number( time[0]) + " sec";
    repo.addText(line);

    line = "Final Time:" + QString::number( time[count -1 ]) + " sec";
    repo.addText(line);

    line = "Time:" + QString::number( time[count-1]-time[0]) + " sec";
    repo.addText(line);


    line = "***********************************************************************";
    repo.addText(line);

    repo.show();

}

void ExportText::makeLine(QString lo, double *v)
{
    QString line = lo;
    line += QString::number(v[2]) + "\t";   // Mean
    line += QString::number(v[3]) + "\t";   // standard deviation
    line += QString::number(v[0]) + "\t";   // Max
    line += QString::number(v[1]) + "\t";   // Min
    line += QString::number(v[0]-v[1]);     // Max-Min
    repo.addText(line);
    file << line.data() << endl;
}

void ExportText::closeFile(){
	file.close();
}

ExportText::~ExportText() {
	// TODO Auto-generated destructor stub
}

}
