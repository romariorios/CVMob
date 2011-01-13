/*
 * FacadeController.cpp
 *
 *  Created on: 20/08/2008
 *      Author: paulista
 */

#include "FacadeController.h"
#include "../model/exporter/ExportText.h"

#include <cassert>

namespace controller {

// initialize instance.
FacadeController *FacadeController::facade = 0;

FacadeController::FacadeController() {

}

/**
 *  return the instace. Used to implement Singleton pattern.
 */
FacadeController *FacadeController::getInstance() {
	if (!facade) {
		facade = new FacadeController();
		facade->calc = new PhisicsCalc();
		return facade;
	}
	return facade;
}

bool FacadeController::openVideo(const QString &fileName) {
	return ProxyOpenCv::getInstance()->openVideo(fileName);
}

bool FacadeController::openCam() {
        return ProxyOpenCv::getInstance()->openCam();
}


bool FacadeController::freeFixPoints() {
        return ProxyOpenCv::getInstance()->freeFixPoints();
}
bool FacadeController::freeTrajPoints() {
        return ProxyOpenCv::getInstance()->freeTrajPoints();
}
bool FacadeController::freeAnglePoints() {
        return ProxyOpenCv::getInstance()->freeAnglePoints();
}

void FacadeController::setShowVectors(bool value, char type){
        ProxyOpenCv::getInstance()->setShowVectors(value, type);
}

void FacadeController::setWinSize(double value){
        ProxyOpenCv::getInstance()->setWinSize(value);
}

double FacadeController::getWinSize(){
        return ProxyOpenCv::getInstance()->getWinSize();
}


void FacadeController::captureFrame(int frame) {
	ProxyOpenCv::getInstance()->Capture(frame);
        ProxyOpenCv::getInstance()->LocatePoints();

        ProxyOpenCv::getInstance()->ShowImage();

}

void FacadeController::setVideoStreamType(int type) {
    ProxyOpenCv::getInstance()->setVideoStreamType(type);
}
void FacadeController::calculateData(int frame) {
        int trajSize,lastPos;
        double x,y;

	for (int i = 0; i < ProxyOpenCv::getInstance()->getCountPoints(); i++) {
		//no frame 3 eu posso calcular a velocidade do frame 2.
		// no frame k eu calculo a velocidade no frame k-1
                trajSize=ProxyOpenCv::getInstance()->points[i].time.size();
                lastPos=trajSize -1;
                if (trajSize >= 3) {
//                        x=ProxyOpenCv::getInstance()->getTrajectX(i, lastPos);
//                        y=ProxyOpenCv::getInstance()->getTrajectY(i, lastPos);
                        ProxyOpenCv::getInstance()->setTrajXY(i, x,y,lastPos);

			//calcular velocidade

			double Vx = calc->calculateAxisVelocity(
                                        ProxyOpenCv::getInstance()->getTrajectDX(i, lastPos - 1),
                                        ProxyOpenCv::getInstance()->getTrajectDX(i, lastPos),
                                        ProxyOpenCv::getInstance()->getTime(lastPos));
			double Vy = calc->calculateAxisVelocity(
                                        ProxyOpenCv::getInstance()->getTrajectDY(i, lastPos - 1),
                                        ProxyOpenCv::getInstance()->getTrajectDY(i, lastPos),
                                        ProxyOpenCv::getInstance()->getTime(lastPos));
                        ProxyOpenCv::getInstance()->setVelocity(i,Vx,Vy, calc->calculateVelocity(Vx, Vy), lastPos);

			//no frame 5 eu posso calcular a aceleração do ponto 3.
			// no frame k eu calculo a aceleração no frame k-2
                        if (trajSize >= 5) {

				//calcular aceleracao
				double Ax = calc->calculateAxisAcceleration(
                                                ProxyOpenCv::getInstance()->getTrajectDX(i, lastPos - 1),
                                                ProxyOpenCv::getInstance()->getTrajectDX(i, lastPos),
                                                ProxyOpenCv::getInstance()->getTrajectDX(i, lastPos - 3),
                                                ProxyOpenCv::getInstance()->getTrajectDX(i, lastPos - 2),
                                                ProxyOpenCv::getInstance()->getTime(lastPos));

				double Ay = calc->calculateAxisAcceleration(
                                                ProxyOpenCv::getInstance()->getTrajectDY(i, lastPos - 1),
                                                ProxyOpenCv::getInstance()->getTrajectDY(i, lastPos),
                                                ProxyOpenCv::getInstance()->getTrajectDY(i, lastPos - 3),
                                                ProxyOpenCv::getInstance()->getTrajectDY(i, lastPos - 2),
                                                ProxyOpenCv::getInstance()->getTime(lastPos));
                                ProxyOpenCv::getInstance()->setAcceleration(i, Ax,Ay,calc->calculateAcceleration(Ax, Ay), lastPos);
			}
		}

	}

}

void FacadeController::exportTrajectory(char* filename) {
	ProxyOpenCv::getInstance()->setExporter(new ExportText());
	ProxyOpenCv::getInstance()->exportData(filename);
}

void FacadeController::exportReport() {
        ProxyOpenCv::getInstance()->setExporter(new ExportText());
        ProxyOpenCv::getInstance()->exportReport();
}
int FacadeController::getTotalFrames() {
	return ProxyOpenCv::getInstance()->getTotalFrames();
}

int FacadeController::getFrameRate() {
	return ProxyOpenCv::getInstance()->getFrameRate();
}

void FacadeController::calculateTime(int actualFrame, char* buff) {
	int min = actualFrame / getFrameRate() / 60;
	double sec = ((double) actualFrame / getFrameRate() - min);
	sprintf(buff, "Time : %02d:%05.2f", min, sec);
}

void FacadeController::setRecording(bool flag)
{
    ProxyOpenCv::getInstance()->setRecording(flag);
}

void FacadeController::startCalibration(double Distance) {
        //	contar quantos pontos existem na tela.
        //	se diferente de 2 dar erro.
        //	se for 2
        ProxyOpenCv::getInstance()->calibrate(Distance);
}

FacadeController::~FacadeController() {
	//	plots.clear();
	//facade = 0;
}

}
