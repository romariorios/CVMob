/*
 * FacadeController.h
 *
 * Controller class. Singleton and Facade pattern.
 * Make the bridge between view and model.
 *
 * comments on .cpp file.
 *
 *  Created on: 20/08/2008
 *      Author: paulista
 */

#ifndef FACADECONTROLLER_H_
#define FACADECONTROLLER_H_

#define MAX_TIME 4000

#include "../model/OpenCV/ProxyOpenCv.h"
#include "../model/PhisicsCalc.h"
#include <qobject.h>
#include <qlist.h>
#include <math.h>


using namespace model;
using namespace std;

namespace controller {

class FacadeController: QObject {
Q_OBJECT
public:

	static FacadeController *getInstance()	;
	bool openVideo(char * fileName);
        bool openCam();
        bool freeFixPoints();
        bool freeTrajPoints();
        bool freeAnglePoints();
	void captureFrame(int frame);
	void calculateData(int frame);
	int getTotalFrames();
	int getFrameRate();
        double getWinSize();
	void calculateTime(int actualFrame, char* buff);
        void setShowVectors(bool value, char type);

	void exportTrajectory(char* filename);
        void exportReport();
        void setVideoStreamType(int type);
        void setRecording(bool flag);


        void startCalibration(double Distance);

	void addVelocity();
	void removeVelocity();
	void addAcceleration();
	void removeAcceleration();
	void addTrabalho();
	void removeTrabalho();
        void setWinSize(double value);

	~FacadeController();
private:
	FacadeController();
	PhisicsCalc *calc;

	static FacadeController *facade;
};

}

#endif /* FACADECONTROLLER_H_ */
