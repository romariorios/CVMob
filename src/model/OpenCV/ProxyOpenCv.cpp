/*
 * cvMo.cpp
 *
 *  Created on: 19/08/2008
 *      Author: paulista
 */

#include "ProxyOpenCv.h"
#include <QWidget>

namespace model {

/**
 * Constructor. Initialize all buffers and variables.
 */
ProxyOpenCv::ProxyOpenCv() {
	this->fileOpen = 0;
	this->frameCount = 0;
	this->frameTotal = 0;

	this->countPoints = 0;
	this->countAngles = 0;
        this->countFixedPoints = 0;
        this->videoStreamType=AVI;
        this->flags = 0;
        this->winSize = 15;

	random = 90;
	horizontalRazao = 1;
	verticalRazao = 1;
	timeRazao = 1;
        this->showVectors=true;
        this->vectorsType=1;        // Velocity is the defalt
	this->countVertices = 0;

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
    this->videoStreamType=type;
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
        if (this->videoStreamType==AVI)
        {
            pt.initFrame=this->frameCount;
            this->rec=true;
        }
        else
            this->rec=false;

	random = random + 200;

        points.push_back(pt);
        // Add the first trajectory point
        if(this->videoStreamType==AVI)
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
            angles[countPt-1].calcValue(this->frameCount/this->frameRate);              // calc angles values and add in trajectory
            emit newAnglePoint();
        }
    }
    ShowImage();
}


/**
 * Capture a frame on position indexFrame.
 */
void ProxyOpenCv::Capture(int indexFrame) {

        this->frameCount = indexFrame;
        if (!this->fileOpen && indexFrame>0) {
            DBG1;DBG2("there is no file open");DBG3;
            return;
        }
        // take the new frame
        if(this->videoStreamType==AVI)
            this->videoStream.set(CV_CAP_PROP_POS_FRAMES,frameCount);
        this->videoStream >> frame;
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
                Size(this->winSize,this->winSize), 3,
                TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 20, 0.01),0.5,0);

            for(unsigned int i=0;i<points.size();i++) // only for trajectory
            {
                points[i].lastPoint=nextPts[i];
                        // store the point on trajectorie array.
                if(this->rec)   // if is recording
                    addTrajPoint(i,frameCount,nextPts[i]);
            }
            int cangle=0,cvert=0;
            for(unsigned int i=points.size();i<status.size();i++) // only for angles
            {
                angles[cangle].vertices[cvert]=nextPts[i];
                cvert++;
                if(cvert==3){cvert=0;
                    angles[cangle++].calcValue((double)frameCount/this->timeRazao);}    // calc angles values and add in trajectory
            }

	}
        CV_SWAP( prev_grey, grey, swap_temp);
}

void ProxyOpenCv::addTrajPoint(int ind,int frame,Point2f pt)
{
    if((frame-this->points[ind].initFrame)<this->points[ind].time.size()-1) // steping back
        return;
    this->points[ind].velocity.push_back(0.0);
    this->points[ind].xVelocity.push_back(0.0);
    this->points[ind].yVelocity.push_back(0.0);

    this->points[ind].traj_x.push_back((double)pt.x*this->horizontalRazao);
    this->points[ind].trajectorie.push_back(pt);
    this->points[ind].traj_y.push_back((double)pt.y*this->verticalRazao);
    this->points[ind].acceleration.push_back(0.0);
    this->points[ind].xAcceleration.push_back(0.0);
    this->points[ind].yAcceleration.push_back(0.0);
    this->points[ind].work.push_back(0.0);
    if(this->videoStreamType==CAM)
        this->points[ind].time.push_back((double)this->ticTac.elapsed()/1000);
    else
        this->points[ind].time.push_back((double)frame/this->timeRazao);
}


//FEATURE:: o certo seria zerar toda a trajetoria e outras coisas.
// se pegarmos outro ponto no mesmo lugar o grafico aind apega.
bool ProxyOpenCv::freeFixPoints() {
        this->countFixedPoints=0;
        this->fixedPoints.clear();
	Capture(frameCount);
	LocatePoints();
	ShowImage();
	return true;
}

bool ProxyOpenCv::freeTrajPoints() {
    for (unsigned int i = 0; i < this->points.size(); i++) {
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
    this->points.clear();
    this->random=90;
    Capture(frameCount);
    LocatePoints();
    ShowImage();
    return true;
}

bool ProxyOpenCv::freeAnglePoints() {

    for (unsigned int i = 0; i < this->angles.size(); i++) {
        angles[i].velocity.clear();
        angles[i].time.clear();
        angles[i].acceleration.clear();
        angles[i].vertices.clear();
        angles[i].value.clear();
        angles[i].initFrame=99999;
    }
    this->angles.clear();
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
        emit updateImage(this->image); // Recebida pela classe imageViwer
}




/**
 * Open a video file
 */
bool ProxyOpenCv::openVideo(char *fileName) {

    this->videoStream.open(fileName);
        if (!(this->videoStream.isOpened())) {
                MSG("Could not initialize capturing...\n A possible encoder problem.\nDownload a codec pack\nlike K-Lite codec");
                return fileOpen=false;
	}

	/**
	 * with no flags to user can resize the video window
	 */
	fileOpen = true;


	/* Determine the number of frames in the AVI. */
        frameTotal = (int) this->videoStream.get(CV_CAP_PROP_FRAME_COUNT) - 1;
	/* Determine the frame Rate in the AVI. */
        frameRate = (int) this->videoStream.get(CV_CAP_PROP_FPS);

	// height and width
        frameSize.height = (int) this->videoStream.get(CV_CAP_PROP_FRAME_HEIGHT );
        frameSize.width = (int) this->videoStream.get(CV_CAP_PROP_FRAME_WIDTH );


	return fileOpen;
}

/**
 * Open a video file
 */
bool ProxyOpenCv::openCam() {

         this->videoStream.open(0);
        if (!(this->videoStream.isOpened())) {
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
        frameRate = (int) this->videoStream.get(CV_CAP_PROP_FPS);

        // height and width
        frameSize.height = (int) this->videoStream.get(CV_CAP_PROP_FRAME_HEIGHT );
        frameSize.width = (int) this->videoStream.get(CV_CAP_PROP_FRAME_WIDTH );

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
        return this->points.size();
}

/**
 */
double ProxyOpenCv::getTrajectX(int point, int pos) {
        return (this->points[point].trajectorie[pos].x) * horizontalRazao;
}

/**
 */
double ProxyOpenCv::getTrajectY(int point, int pos) {
        return (this->points[point].trajectorie[pos].y) * verticalRazao;
}

double ProxyOpenCv::getTrajectDX(int point, int pos) {
        return (this->points[point].trajectorie[pos].x
              - this->points[point].trajectorie[pos-1].x) * horizontalRazao;
}

/**
 */
double ProxyOpenCv::getTrajectDY(int point, int pos) {
        return (this->points[point].trajectorie[pos].y
              - this->points[point].trajectorie[pos-1].y) * verticalRazao;
}


/**
 * set the flag ShowVectors
 */
void ProxyOpenCv::setShowVectors(bool value, char type) {
this->showVectors=value;
this->vectorsType=type;
this->Capture(frameCount);
this->ShowImage();
}

/**
 * set the window size for search optical flow
 */
void ProxyOpenCv::setWinSize(double value) {
this->winSize=value;
this->Capture(frameCount);
this->ShowImage();
}

/**
 * returns the window size for search optical flow
 */
double ProxyOpenCv::getWinSize() {
  return this->winSize;
}

/**
 * return a velocity point in a frame.
 */
double ProxyOpenCv::getVelocity(int point, int frame) {
        return points[point].velocity[frame-this->points[point].initFrame];
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
//    if(this->videoStreamType==CAM)
//        points[point].time[pos] = (double)this->ticTac.elapsed()/1000;
//    else
//        points[point].time[pos] = this->frameCount / timeRazao;

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
    if(this->videoStreamType==CAM)
    {
        if(pos>0)
            dt=this->points[0].time[pos] - this->points[0].time[pos-1];
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
        if (this->getCountPoints() != 2) {
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
	this->exporter = exporter;
}

void ProxyOpenCv::exportData(char* filename) {

	this->exporter->setFile(filename);
        for (int i = 0; i < this->getCountPoints(); i++) {
                this->exporter->exportData(points[i].time.size(), points[i].time,
                                points[i].velocity, points[i].acceleration, points[i].work,
                                points[i].trajectorie, horizontalRazao, verticalRazao,points[i].initFrame,
                                points[i].xVelocity,points[i].yVelocity,points[i].xAcceleration,points[i].yAcceleration);
	}
}
void ProxyOpenCv::exportReport() {

        for (int i = 0; i < this->getCountPoints(); i++) {
                this->exporter->exportReport(points[i].time.size(), points[i].time,
                                points[i].velocity, points[i].acceleration, points[i].work,
                                points[i].trajectorie, horizontalRazao, verticalRazao,points[i].initFrame,
                                points[i].xVelocity,points[i].yVelocity,points[i].xAcceleration,points[i].yAcceleration);
        }
}

/**
 * destructor.
 */
ProxyOpenCv::~ProxyOpenCv() {
	if (!this->fileOpen) {
		cout << "problem" << endl;
		return;
	}
	opencv = 0;
}



float ProxyOpenCv::get_horizontalRazao()
{
    return this->horizontalRazao;
}

float ProxyOpenCv::get_verticalRazao()
{
    return this->verticalRazao;
}

int ProxyOpenCv::get_countPoints()
{
    return this->points.size();
}

void ProxyOpenCv::setRecording(bool flag)
{
    this->rec=flag;
    if(flag)
        this->ticTac.restart();
}


}


