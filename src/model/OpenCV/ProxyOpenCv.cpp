/*
 * cvMo.cpp
 *
 *  Created on: 19/08/2008
 *      Author: paulista
 */

#include "ProxyOpenCv.h"
#include <QWidget>

#include <QDebug>

namespace model {

/**
 * Constructor. Initialize all buffers and variables.
 */
ProxyOpenCv::ProxyOpenCv() {
	fileOpen = 0;
	frameCount = 0;
	frameTotal = 0;

	countPoints = 0;
	countAngles = 0;
		countFixedPoints = 0;
		videoStreamType=AVI;
		flags = 0;
		winSize = 15;

	random = 90;
	horizontalRazao = 1;
	verticalRazao = 1;
	timeRazao = 1;
		showVectors=true;
		vectorsType=1;        // Velocity is the defalt
	countVertices = 0;

}

// initialize instance.
ProxyOpenCv *ProxyOpenCv::opencv = 0;

/**
 *  return the instace. Used to implement Singleton pattern.
 */
ProxyOpenCv *ProxyOpenCv::getInstance() {
	if (!opencv) {
		opencv = new ProxyOpenCv();
		return opencv;
	}
	return opencv;
}

/**
 * Set the type of Stream if is CAM or AVI.
 */
void ProxyOpenCv::setVideoStreamType(int type)
{
    videoStreamType=type;
}

/**
 * Store Point marked on screen.
 */
void ProxyOpenCv::captureTrajectoriePoint(Point2f poin) {
        point pt;
        int countPt=points.size();
        pt.markedPoint = poin;
        pt.lastPoint =  poin;
        pt.index = countPt;

        pt.color[0] = random % 255; //random mod 255
        pt.color[1] = (random * 2) % 255; //random * 2 mod 255
        pt.color[2] = (random * 3) % 255; // random *3 mod 255
        pt.color[3] = 0; // research
        if (videoStreamType==AVI)
        {
            pt.initFrame=frameCount;
            rec=true;
        }
        else
            rec=false;

	random = random + 200;

        points.push_back(pt);
        // Add the first trajectory point
        if(videoStreamType==AVI)
            addTrajPoint(points.size()-1,frameCount,poin);

        emit
        addCurve(points[countPt].index, points[countPt].color[0],
                        points[countPt].color[1], points[countPt].color[2]);
        emit newTrajPoint();

	// repaint the image to show the points
	ShowImage();
}

void ProxyOpenCv::captureFixedPoint(CvPoint2D32f point) {
    FixedPoint pt;
    pt.markedPoint=point;
    fixedPoints.push_back(pt);
        countFixedPoints++;
	ShowImage();
}

void ProxyOpenCv::captureVerticePoint(Point2f poin) {
    Angle pt;
    int countPt=angles.size();
    if(countPt==0 || angles[countPt-1].vertices.size()==3) // new point
    {
        pt.markedPoint = poin;
        pt.index = countPt;

        pt.color[0] = 255;
        pt.color[1] = 0;
        pt.color[2] = 0;
        pt.color[3] = 0;
        pt.vertices.push_back(poin);
        angles.push_back(pt);
    }
    else
    {
        angles[countPt-1].vertices.push_back(poin);
        if(angles[countPt-1].vertices.size()==3)
        {
            angles[countPt-1].calcValue(frameCount/frameRate);              // calc angles values and add in trajectory
            emit newAnglePoint();
        }
    }
    ShowImage();
}


/**
 * Capture a frame on position indexFrame.
 */
void ProxyOpenCv::Capture(int indexFrame) {

        frameCount = indexFrame;
        if (!fileOpen && indexFrame>0) {
            DBG1;DBG2("there is no file open");DBG3;
            return;
        }
        // take the new frame
        if(videoStreamType==AVI)
            videoStream.set(CV_CAP_PROP_POS_FRAMES,frameCount);
        videoStream >> frame;
        if( frame.empty() ) return;
        cvtColor(frame,image,CV_BGRA2RGB);
        cvtColor(image, grey, CV_RGB2GRAY);
}


/**
 * Calculates optical flow running Pyramidal Lucas Kanade Optical Flow.
 * This function calculates LK for each point stored.
 */
void ProxyOpenCv::LocatePoints() {
        if ((frameCount > 1) && (points.size() > 0 || angles.size()>0)) {
            vector<Point2f> prevPts;
            vector<Point2f> nextPts;
            vector<uchar> status;
            vector<float> err;
            // Add trajectory points
            for(unsigned int i=0;i<points.size();i++)
                prevPts.push_back(points[i].lastPoint);

            // Add angle points
            for(unsigned int i=0;i<angles.size();i++)
                for(unsigned int j=0;j<angles[i].vertices.size();j++)
                    prevPts.push_back(angles[i].vertices[j]);

            calcOpticalFlowPyrLK( prev_grey, grey, prevPts,nextPts, status,err,
                Size(winSize,winSize), 3,
                TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 20, 0.01),0.5,0);

            for(unsigned int i=0;i<points.size();i++) // only for trajectory
            {
                points[i].lastPoint=nextPts[i];
                        // store the point on trajectorie array.
                if(rec)   // if is recording
                    addTrajPoint(i,frameCount,nextPts[i]);
            }
            int cangle=0,cvert=0;
            for(unsigned int i=points.size();i<status.size();i++) // only for angles
            {
                angles[cangle].vertices[cvert]=nextPts[i];
                cvert++;
                if(cvert==3){cvert=0;
                    angles[cangle++].calcValue((double)frameCount/timeRazao);}    // calc angles values and add in trajectory
            }

	}
        CV_SWAP( prev_grey, grey, swap_temp);
}

void ProxyOpenCv::addTrajPoint(int ind,int frame,Point2f pt)
{
    if((frame-points[ind].initFrame)<points[ind].time.size()-1) // steping back
        return;
    points[ind].velocity.push_back(0.0);
    points[ind].xVelocity.push_back(0.0);
    points[ind].yVelocity.push_back(0.0);

    points[ind].traj_x.push_back((double)pt.x*horizontalRazao);
    points[ind].trajectorie.push_back(pt);
    points[ind].traj_y.push_back((double)pt.y*verticalRazao);
    points[ind].acceleration.push_back(0.0);
    points[ind].xAcceleration.push_back(0.0);
    points[ind].yAcceleration.push_back(0.0);
    points[ind].work.push_back(0.0);
    if(videoStreamType==CAM)
        points[ind].time.push_back((double)ticTac.elapsed()/1000);
    else
        points[ind].time.push_back((double)frame/timeRazao);
}


//FEATURE:: o certo seria zerar toda a trajetoria e outras coisas.
// se pegarmos outro ponto no mesmo lugar o grafico aind apega.
bool ProxyOpenCv::freeFixPoints() {
        countFixedPoints=0;
        fixedPoints.clear();
	Capture(frameCount);
	LocatePoints();
	ShowImage();
	return true;
}

bool ProxyOpenCv::freeTrajPoints() {
    for (unsigned int i = 0; i < points.size(); i++) {
        points[i].velocity.clear();
        points[i].xVelocity.clear();
        points[i].yVelocity.clear();
        points[i].time.clear();
        points[i].acceleration.clear();
        points[i].xAcceleration.clear();
        points[i].yAcceleration.clear();
        points[i].work.clear();
        points[i].trajectorie.clear();
        points[i].traj_x.clear();
        points[i].traj_y.clear();

        points[i].initFrame=99999;
    }
    points.clear();
    random=90;
    Capture(frameCount);
    LocatePoints();
    ShowImage();
    return true;
}

bool ProxyOpenCv::freeAnglePoints() {

    for (unsigned int i = 0; i < angles.size(); i++) {
        angles[i].velocity.clear();
        angles[i].time.clear();
        angles[i].acceleration.clear();
        angles[i].vertices.clear();
        angles[i].value.clear();
        angles[i].initFrame=99999;
    }
    angles.clear();
    Capture(frameCount);
    LocatePoints();
    ShowImage();
    return true;
}

/**
 * ShowImage captured on Capture function.
 * if there is points, then this function draw points first.
 */
void ProxyOpenCv::ShowImage() {
        emit updateImage(image); // Recebida pela classe imageViwer
}




/**
 * Open a video file
 */
bool ProxyOpenCv::openVideo(const QString &fileName) {

	videoStream.open(fileName.toStdString());
	if (!videoStream.isOpened()) {
                MSG("Could not initialize capturing...\n A possible encoder problem.\nDownload a codec pack\nlike K-Lite codec");
                return fileOpen=false;
	}

	/**
	 * with no flags to user can resize the video window
	 */
	fileOpen = true;


	/* Determine the number of frames in the AVI. */
		frameTotal = (int) videoStream.get(CV_CAP_PROP_FRAME_COUNT) - 1;
	/* Determine the frame Rate in the AVI. */
		frameRate = (int) videoStream.get(CV_CAP_PROP_FPS);

	// height and width
		frameSize.height = (int) videoStream.get(CV_CAP_PROP_FRAME_HEIGHT );
		frameSize.width = (int) videoStream.get(CV_CAP_PROP_FRAME_WIDTH );


	return fileOpen;
}

/**
 * Open a video file
 */
bool ProxyOpenCv::openCam() {

         videoStream.open(0);
        if (!(videoStream.isOpened())) {
                MSG("Could not initialize capturing...\n");
                return fileOpen=false;
        }
        /**
         * with no flags to user can resize the video window
         */
        fileOpen = true;


        /* Determine the number of frames in the AVI. */
        frameTotal = 0;
        /* Determine the frame Rate in the AVI. */
        frameRate = (int) videoStream.get(CV_CAP_PROP_FPS);

        // height and width
        frameSize.height = (int) videoStream.get(CV_CAP_PROP_FRAME_HEIGHT );
        frameSize.width = (int) videoStream.get(CV_CAP_PROP_FRAME_WIDTH );

        return fileOpen;
}


/**
 * return the total number of frames on the video.
 */
int ProxyOpenCv::getTotalFrames() {
	return frameTotal;
}

/**
 * return the frame rate on the video.
 */
int ProxyOpenCv::getFrameRate() {
	return frameRate;
}

/**
 * return number of points.
 */
int ProxyOpenCv::getCountPoints() {
        return points.size();
}

/**
 */
double ProxyOpenCv::getTrajectX(int point, int pos) {
        return (points[point].trajectorie[pos].x) * horizontalRazao;
}

/**
 */
double ProxyOpenCv::getTrajectY(int point, int pos) {
        return (points[point].trajectorie[pos].y) * verticalRazao;
}

double ProxyOpenCv::getTrajectDX(int point, int pos) {
        return (points[point].trajectorie[pos].x
              - points[point].trajectorie[pos-1].x) * horizontalRazao;
}

/**
 */
double ProxyOpenCv::getTrajectDY(int point, int pos) {
        return (points[point].trajectorie[pos].y
              - points[point].trajectorie[pos-1].y) * verticalRazao;
}


/**
 * set the flag ShowVectors
 */
void ProxyOpenCv::setShowVectors(bool value, char type) {
showVectors=value;
vectorsType=type;
Capture(frameCount);
ShowImage();
}

/**
 * set the window size for search optical flow
 */
void ProxyOpenCv::setWinSize(double value) {
winSize=value;
Capture(frameCount);
ShowImage();
}

/**
 * returns the window size for search optical flow
 */
double ProxyOpenCv::getWinSize() {
  return winSize;
}

/**
 * return a velocity point in a frame.
 */
double ProxyOpenCv::getVelocity(int point, int frame) {
        return points[point].velocity[frame-points[point].initFrame];
}
int ProxyOpenCv::getInitFrame(int point) {
        return points[point].initFrame;
}


/**
 * set velocity in the determined frame
 */
void ProxyOpenCv::setVelocity(int point,double Vx,double Vy, double velocity, int pos) {

        points[point].velocity[pos] = velocity;
        points[point].xVelocity[pos] = Vx;
        points[point].yVelocity[pos] = Vy;

        if(pos)
            emit updateVelocity(points[point].index, points[point].time,
                        points[point].velocity, pos);
}


/**
 * set Trajectory in the determined frame (For graph pruposes)
 */
void ProxyOpenCv::setTrajXY(int point, double x, double y, int pos)
{
//    points[point].traj_x[pos]=x;
//    points[point].traj_y[pos]=y;
//    if(videoStreamType==CAM)
//        points[point].time[pos] = (double)ticTac.elapsed()/1000;
//    else
//        points[point].time[pos] = frameCount / timeRazao;

    if(pos)
    {
        emit updateTraj_x(points[point].index, points[point].time,
                          points[point].traj_x, pos);
        emit updateTraj_y(points[point].index, points[point].time,
                          points[point].traj_y, pos);
    }
}
/**
 * set acceleration in the determined frame
 */
void ProxyOpenCv::setAcceleration(int point,double Ax,double Ay, double acceleration, int pos) {
        points[point].acceleration[pos] = acceleration;
        points[point].xAcceleration[pos] = Ax;
        points[point].yAcceleration[pos] = Ay;
        if(pos)
            emit updateAcceleration(points[point].index, points[point].time,
                        points[point].acceleration, pos);
}

/**
 * return a velocity point in a frame.
 */
double ProxyOpenCv::getAcceleration(int point, int pos) {
        return points[point].acceleration[pos];
}


/**
 * return a razao 1 frame/frame rate. Returns the seconds relative a 1 frame.
 */
double ProxyOpenCv::getTime(int pos) {
    double dt;
    if(videoStreamType==CAM)
    {
        if(pos>0)
            dt=points[0].time[pos] - points[0].time[pos-1];
        else dt=0;
        return dt;
    }else
	return 1 / timeRazao;
}

/**
 * Calibrate camera. Find a relation beetwen Frames and seconds.
 * BUG: markedPoint was coming zero. Investigate this.
 * Using prev_features for now.
 */
void ProxyOpenCv::calibrate(double Distance) {
        // we need 2 points on the screen to calibrate
        if (getCountPoints() != 2) {
		emit calibrateOk(false);
		return;
	}
	//	initialize variables with values from a point
        double dx = points[1].lastPoint.x-points[0].lastPoint.x;
        double dy = points[1].lastPoint.y-points[0].lastPoint.y;
        double d=sqrt(dx*dx+dy*dy);
        horizontalRazao = (Distance / d) / 100;
        verticalRazao = horizontalRazao;
	timeRazao = frameRate;
	emit calibrateOk(true);
}

void ProxyOpenCv::setExporter(IExportStrategy* exporter) {
	exporter = exporter;
}

void ProxyOpenCv::exportData(char* filename) {

    exporter->setFile(filename);
        for (int i = 0; i < getCountPoints(); i++) {
                exporter->exportData(points[i].time.size(), points[i].time,
                                points[i].velocity, points[i].acceleration, points[i].work,
                                points[i].trajectorie, horizontalRazao, verticalRazao,points[i].initFrame,
                                points[i].xVelocity,points[i].yVelocity,points[i].xAcceleration,points[i].yAcceleration);
	}
}
void ProxyOpenCv::exportReport() {

        for (int i = 0; i < getCountPoints(); i++) {
                exporter->exportReport(points[i].time.size(), points[i].time,
                                points[i].velocity, points[i].acceleration, points[i].work,
                                points[i].trajectorie, horizontalRazao, verticalRazao,points[i].initFrame,
                                points[i].xVelocity,points[i].yVelocity,points[i].xAcceleration,points[i].yAcceleration);
        }
}

/**
 * destructor.
 */
ProxyOpenCv::~ProxyOpenCv() {
	if (!fileOpen) {
		cout << "problem" << endl;
		return;
	}
	opencv = 0;
}



float ProxyOpenCv::get_horizontalRazao()
{
    return horizontalRazao;
}

float ProxyOpenCv::get_verticalRazao()
{
    return verticalRazao;
}

int ProxyOpenCv::get_countPoints()
{
    return points.size();
}

void ProxyOpenCv::setRecording(bool flag)
{
    rec=flag;
    if(flag)
        ticTac.restart();
}


}


