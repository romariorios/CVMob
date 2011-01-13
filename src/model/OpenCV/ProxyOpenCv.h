/*
 * cvMo.h 3
 *
 *  Created on: 19/08/2008
 *      Author: paulista
 */


//TODO: tentar calcular a proxima imagem apenas com o ponto que está pra ser procurado no frame anterior.
// tentar diminuir o erro porque o movimento vai e volta pelo mesmo lugar.

#ifndef PROXYOPENCV_H_
#define PROXYOPENCV_H_
#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <ctype.h>
#endif
#include "../exporter/IExportStrategy.h"


#include "Point.h"
#include "Angle.h"

#define MAX_POINTS  10
#define MAX_FIX_POINTS  100
#define MAX_TIME  4000
#define FPS 25
#define DEBUG "CvMobVideo"

#include <qobject.h>
#include <QWidget>
#include <iostream>
#include <QtGui>
#include <vector>
#include <QVector>
#include <QMessageBox>

#define DBG1 QMessageBox *message = new QMessageBox()
#define DBG2(x) message->setText(x);
#define DBG3 message->show();
#define MSG(x) QMessageBox *message = new QMessageBox();message->setText(x);int _r=message->exec()
#define MSG2(x) message->setText(x); _r=message->exec()

using namespace std;
using namespace cv;
enum method {AVI, CAM};

namespace model {

class ProxyOpenCv : public QWidget {
	Q_OBJECT
private:
        VideoCapture videoStream;
        Mat frame;
	CvFont font;
        QTime ticTac;

	CvSize frameSize;

	int countPoints;
	int countAngles;
        int countFixedPoints;

	int random;

        void addTrajPoint(int ind,int frame,Point2f pt);
	static ProxyOpenCv *opencv;
	char* screenName;
        bool rec;       // flag indicating recording
	bool fileOpen; // flag indicates a open file.
	int frameCount;
	int frameTotal;
	int frameRate;
	int flags;

        Mat  grey, prev_grey, swap_temp;
	// another copy to avoid too much points


	float horizontalRazao;
	float verticalRazao;
	float timeRazao;
        int videoStreamType;
	IExportStrategy *exporter;

	// buff to store the formation angles.
	CvPoint2D32f vertices[3];
	int countVertices;

public:
        float get_horizontalRazao();
        float get_verticalRazao();
        int get_countPoints();
        void setVideoStreamType(int type);
        void setRecording(bool flag);

	ProxyOpenCv();
        Mat image;
        char vectorsType;       // 1 for velocity and 0 for acceleration
        double winSize;        // Janela da busca para fluxo otico
        bool showVectors;

        vector <point> points;
        vector <FixedPoint> fixedPoints;
        vector <Angle> angles;
        CvPoint pt;
	static ProxyOpenCv *getInstance();

	void goodFeaturesToTrack();

        void captureTrajectoriePoint(Point2f point);
        void captureFixedPoint(CvPoint2D32f point);
        void captureVerticePoint(Point2f point);
	bool openVideo(const QString &fileName);
        bool openCam();

        bool freeFixPoints();
        bool freeTrajPoints();
        bool freeAnglePoints();

	void Capture(int indexFrame);
	int getTotalFrames();
	int getCountPoints();
	int getFrameRate();
	void LocatePoints();
	void ShowImage();

        double getTrajectDX(int point, int frame);
        double getTrajectDY(int point, int frame);
        double getTrajectX(int point, int frame);
        double getTrajectY(int point, int frame);
        void setTrajXY(int point, double x, double y, int frame);

        void setVelocity(int point,double Vx,double Vy, double velocity, int frame);
	double getVelocity(int point, int frame);
        int getInitFrame(int point);

        void setAcceleration(int point, double Ax,double Ay, double acceleration, int frame);
	double getAcceleration(int point, int frame);

        double getTime(int frame);

        void calibrate(double Distance);

	void setExporter(IExportStrategy* exporter);
	void exportData(char* filename);
        void exportReport();

        void setShowVectors(bool value,char type);

        void setWinSize(double value);
        double getWinSize();
	~ProxyOpenCv();

signals:
	void addCurve(int index, int r, int g, int b);
	void calibrateOk(bool);
        void updateVelocity(int index, QVector <double> time, QVector <double> velocity, int actualTime);
        void updateAcceleration(int index, QVector <double> time, QVector <double> acceleration, int actualTime);
        void updateWork(int index, QVector <double> time, QVector <double> work, int actualTime);
        void updateTraj_x(int index, QVector <double> time, QVector <double> traj_x, int actualTime);
        void updateTraj_y(int index, QVector <double> time, QVector <double> traj_y, int actualTime);
        void updateImage(Mat image);
        void newTrajPoint();
        void newAnglePoint();
};

}

#endif /* PROXYOPENCV_H_ */
