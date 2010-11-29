/*
 * cvMo.cpp
 *
 *  Created on: 19/08/2008
 *      Author: paulista
 */

#include "cvMo.h"
#include <stdio.h>

namespace model {

double cvMo::GetTrajectX(int point, int time) {
	return this->pts[point][time].x;
}

double cvMo::GetTrajectY(int point, int time) {
	return this->pts[point][time].y;
}

int cvMo::GetNTraject(int i) {
	return this->pti[i];
}

char cvMo::GetpTipo(int i) {
	return this->ptTipo[i];
}

int cvMo::GetPCount() {
	return this->count;
}

void cvMo::PutText(IplImage *image, char *text, int x, int y, CvFont *font,
		int R, int G, int B) {
	CvPoint ax;
	ax.x = x;
	ax.y = y;
	cvPutText(image, text, ax, font, CV_RGB(R, G, B));
}
void cvMo::SetPlay(char fl) {
	this->FlPlay = fl;
}

void cvMo::SetGrid(char fl) {
	this->FlGrid = fl;
}

void cvMo::SetStep(char fl) {
	this->FlStep = fl;
	if (!this->FlisOpen)
		return;
	this->Fc += fl;
	if (this->Fc >= this->TotFrames - 1)
		this->Fc = this->TotFrames - 1;
}

void cvMo::on_mouseStatic(int event, int x, int y, int flags, void* param) {
	static_cast<cvMo*> (param)->on_mouse(event, x, y, flags);
}

void cvMo::on_mouse(int event, int x, int y, int flags) {
	CvPoint pt;
	if (!(this->image))
		return;

	if (this->image->origin)
		y = this->image->height - y;

	if (event == CV_EVENT_RBUTTONDOWN) {
		pt = cvPoint(x, y);
		this->add_remove_pt = 1;
		this->points[0][this->count] = cvPointTo32f(pt);
		this->ptTipo[this->count] = 2; // 2 indica ponto de angulo
		this->pti[this->count] = 1; // inicia contador de tamanho de trajetoria
		this->pts[this->count][0] = cvPointTo32f(pt);
		this->countA++;
		this->count++;
	}

	if (event == CV_EVENT_LBUTTONDOWN) {
		cout << "left button" << endl;
		pt = cvPoint(x, y);
		this->add_remove_pt = 1;
		this->points[0][count] = cvPointTo32f(pt);
		this->ptTipo[count] = 1;
		this->pti[count] = 1; // zera contador de tamanho de trajetoria
		this->pts[count][0] = cvPointTo32f(pt);
		this->count++;
	}
	this->LocatePoints();
	this->ShowImage();
}

cvMo::cvMo() {
	this->Rec = 0; // Is not recording
	this->capture = 0;
	this->frame = 0;
	this->Fc = 0; // Zera contador de frames
	//	this->image = 0;
	this->grey = 0;
	this->prev_grey = this->pyramid = this->prev_pyramid = 0;
	this->flags = 0;
	this->count = 0;
	this->countA = 0;
	this->countAG = 0;
	this->status = 0;
	this->add_remove_pt = 0;
	this->add_remove_ag = 0;
	//	this->points[2] = {0,0};
	this->win_size = 10;
	this->NFrame = 0;
	this->FilePos = 0;
	this->FlPlay = 0;
	this->FlStep = 0;
	this->FlGrid = 0;
	this->FlisOpen = 0;
	this->FlName[0] = 0;
	this->NApoints = 0;
}


// initialize instance.
cvMo *cvMo::opencv = 0;


/**
 *  return the instace. Used to implement Singleton pattern.
 */
cvMo *cvMo::getInstance(){
	if (!opencv){
		opencv = new cvMo();
		return opencv;
	}
	return opencv;
}



cvMo::~cvMo() {
	if (!this->FlisOpen)
		return;
	cvDestroyAllWindows();
	if (this->FlRec)
		fclose(this->FlRec);
}

bool cvMo::ThereIsAngle() {
	if (this->countA == 3)
		return true;
	else
		return false;
}

int cvMo::Init() {
	//	cvNamedWindow( "LevantaIan", 0 );

	cvInitFont(&(this->font), CV_FONT_VECTOR0, 0.8, 0.8, 0.0, 2, 8);
	this->FlisOpen = 0; // indicates that there is no file opened
	this->flags = 0;
	return 1;
}

int cvMo::Init(char *file) {
	this->capture = cvCaptureFromAVI(file);
	if (!(this->capture)) {
		fprintf(stderr, "Could not initialize capturing...\n");
		cvReleaseCapture(&capture);
		return 0;
	}

	cvNamedWindow("LevantaIan", 0);
	cvSetMouseCallback("LevantaIan", cvMo::on_mouseStatic, this);
	this->TotFrames = (int) cvGetCaptureProperty(this->capture,
			CV_CAP_PROP_FRAME_COUNT) - 1;
	this->FrameWidth = (int) cvGetCaptureProperty(this->capture,
			CV_CAP_PROP_FRAME_WIDTH);
	this->FrameHeight = (int) cvGetCaptureProperty(this->capture,
			CV_CAP_PROP_FRAME_HEIGHT);
	this->FrameRate
			= (int) cvGetCaptureProperty(this->capture, CV_CAP_PROP_FPS);
	cvResizeWindow("LevantaIan", this->FrameWidth, this->FrameHeight);

	this->GridStepW = (int) (0.1 * this->FrameWidth);
	this->GridStepH = (int) (0.1 * this->FrameHeight);

	this->frame = cvQueryFrame(this->capture);

	cvInitFont(&(this->font), CV_FONT_VECTOR0, 0.8, 0.8, 0.0, 2, 8);

	/* allocate all the buffers */
	this->image = cvCreateImage(cvGetSize(this->frame), 8, 3);
	this->image->origin = this->frame->origin;
	this->grey = cvCreateImage(cvGetSize(this->frame), 8, 1);
	this->prev_grey = cvCreateImage(cvGetSize(this->frame), 8, 1);
	this->pyramid = cvCreateImage(cvGetSize(this->frame), 8, 1);
	this->prev_pyramid = cvCreateImage(cvGetSize(this->frame), 8, 1);
	this->points[0] = (CvPoint2D32f*) cvAlloc(MAX_COUNT
			* sizeof(this->points[0][0]));
	this->points[1] = (CvPoint2D32f*) cvAlloc(MAX_COUNT
			* sizeof(this->points[0][0]));
	this->ptTipo = (char*) malloc(MAX_COUNT * sizeof(char));

	this->status = (char*) cvAlloc(MAX_COUNT);
	this->flags = 0;
	this->FlisOpen = 1;
	return 1;
}

char cvMo::IsRecording() {
	return this->Rec;
}

char cvMo::IsStepping() {
	return this->FlStep;
}
void cvMo::Record(FILE *Frg) {
	this->FlRec = Frg; // Guarda ponteiro do arquivo para salvar trajetorias
	this->Rec = 1; // Set Flag
}
void cvMo::StopRecord() {
	this->Rec = 0; // Set Flag
}



bool cvMo::freePoints() {
	int i;
	if (!this->FlisOpen)
		return false;
	for (i = 0; i < this->count; i++) {
		this->pti[i] = this->ptTipo[i] = 0;
	}
	this->count = this->countAG = this->countA = this->NApoints = 0;
	cout << "free points" << endl;
	return true;
}

bool cvMo::freeTrajectories() {
	int i;
	for (i = 0; i < this->count; i++)
		this->pti[i] = 0;
	this->NApoints = 0;
	cout << "free trajectories" << endl;
	return true;
}

void cvMo::Capture(int Step) {
	this->Fc = Step;
	if (!this->FlisOpen)
		return;
	cvSetCaptureProperty(this->capture, CV_CAP_PROP_POS_FRAMES, this->Fc);
	this->frame = cvQueryFrame(this->capture);
	cvSetTrackbarPos("Frame", screenName, this->Fc);
	cvCopy(this->frame, this->image, 0);
	cvCvtColor(this->image, this->grey, CV_BGR2GRAY);
}

void cvMo::SetGridStep(int p) {
	this->GridStepW = p;
	this->GridStepH = p;
}

void cvMo::LocatePoints() {
	int i;
	char FlagStat = 0;

	if (this->count > 0 && this->Fc != this->FcA) {
		this->FcA = this->Fc;
		cvCalcOpticalFlowPyrLK(this->prev_grey, this->grey, this->prev_pyramid,
				this->pyramid, this->points[0], this->points[1], this->count,
				cvSize(this->win_size, this->win_size), 3, this->status, 0,
				cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03),
				this->flags);
		this->flags |= CV_LKFLOW_PYR_A_READY;
		for (i = 0; i < this->count; i++) {
			if (this->status[i]) // Se o fluxo  foi encontrado guarda ponto
			{
				this->pts[i][this->pti[i]] = this->points[1][i];
				this->pti[i]++;
				FlagStat = 1;

			} else
				continue;
		}
		for (i = 0; i < this->count; i++) {
			if (this->ptTipo[i] == 2 && this->countA == 3) {
				this->Ang1 = this->CalcAngG((double) this->points[0][i + 1].x,
						(double) this->points[0][i + 1].y,
						(double) this->points[0][i].x, (double) points[0][i].y);
				this->Ang2 = this->CalcAngG((double) this->points[0][i + 1].x,
						(double) this->points[0][i + 1].y,
						(double) this->points[0][i + 2].x,
						(double) this->points[0][i + 2].y);
				this->Ang[this->NApoints] = this->Ang1 - this->Ang2;
				this->TimeA[NApoints] = this->Fc;
				NApoints++;
				i += 2;
			}
		}
		CV_SWAP(this->prev_grey, this->grey, this->swap_temp);
		CV_SWAP(this->prev_pyramid, this->pyramid, this->swap_temp);
		// se almenos um ponto foi achado troca pontos
		if (FlagStat){
			CV_SWAP(this->points[0], this->points[1], this->swap_points);
		}
	}
	if (this->FlGrid)
		this->DrawGrid();

	cvWaitKey(10);

}

void cvMo::ShowImage() {
	char texto[190];
	if (!this->FlisOpen)
		return;

	for (int i = 0; i < this->count; i++) {
		if (this->ptTipo[i] == 1) // Trajetoria
		{
			cvCircle(this->image, cvPointFrom32f(this->points[0][i]), 3,
					CV_RGB(0, 255, 0), -1, 8, 0);
			for (int ip = 0; ip < this->pti[i]; ip++)
				cvCircle(this->image, cvPointFrom32f(this->pts[i][ip]), 2,
						CV_RGB(250, 4, 190), -1, 8, 0);
		} else // Ponto tipo angulo
		{
			cvCircle(this->image, cvPointFrom32f(this->points[0][i]), 3,
					CV_RGB(255, 0, 0), -1, 8, 0);
			if (this->countA == 3) // Atingiu o numero suficiente para calculo de um angulo
			{
				this->Ang1 = this->CalcAngG((double) this->points[0][i + 1].x,
						(double) this->points[0][i + 1].y,
						(double) this->points[0][i].x, (double) points[0][i].y);
				this->Ang2 = this->CalcAngG((double) this->points[0][i + 1].x,
						(double) this->points[0][i + 1].y,
						(double) this->points[0][i + 2].x,
						(double) this->points[0][i + 2].y);
				cvCircle(this->image, cvPointFrom32f(this->points[0][i + 1]),
						3, CV_RGB(255, 0, 0), -1, 8, 0);
				cvCircle(this->image, cvPointFrom32f(this->points[0][i + 2]),
						3, CV_RGB(255, 0, 0), -1, 8, 0);
				cvLine(this->image, cvPointFrom32f(this->points[0][i]),
						cvPointFrom32f(this->points[0][i + 1]), CV_RGB(0, 0,
								255), 1, 8, 0);
				cvLine(this->image, cvPointFrom32f(this->points[0][i + 1]),
						cvPointFrom32f(this->points[0][i + 2]), CV_RGB(0, 0,
								255), 1, 8, 0);
				sprintf(texto, "%3do", (int) (this->Ang1 - this->Ang2));
				cvPutText(this->image, texto, cvPointFrom32f(this->points[0][i
						+ 1]), &(this->font), CV_RGB(255, 50, 0));
				i += 2; // Salta os proximos pontos do angulo
			}
		}
	}
	cvShowImage(screenName, this->image);
	cvWaitKey(10);


}

void cvMo::DrawGrid() {
	int i;
	CvPoint ax1, ax2;
	if (!this->FlisOpen)
		return;
	for (i = 0; i <= this->FrameWidth; i += this->GridStepW) {
		ax1.x = i;
		ax1.y = 0;
		ax2.x = i;
		ax2.y = this->FrameHeight;
		cvLine(this->image, ax1, ax2, CV_RGB(90, 255, 255), 1, 8, 0);
	}
	for (i = 0; i <= this->FrameHeight; i += this->GridStepH) {
		ax1.x = 0;
		ax1.y = i;
		ax2.x = this->FrameWidth;
		ax2.y = i;
		cvLine(this->image, ax1, ax2, CV_RGB(90, 255, 255), 1, 8, 0);
	}
}
double cvMo::CalcAngG(double Xc, double Yc, double X, double Y) {
	double AngR, AngG, dx, dy;
	dx = X - Xc;
	dy = Y - Yc;
	if (dx == 0 && dy == 0)
		return 0.0;
	//	if(dy==0) return 90;
	AngR = atan2(dy, dx);
	AngG = (360.0 / (2.0 * 3.1416)) * AngR;
	return AngG;
}

FILE* cvMo::AbreArquivo(char *name) {
	FILE *fp;
	static int Nno = 1;
	char newName[100];
	int CntPT = 1; //contador de pontos de trajetoria
	int CntPA = 1; //contador de pontos de angulos
	int i;

	sprintf(newName, "%s%02d.dat", name, Nno++);
	fp = fopen(newName, "w");
	// Imprime cabecalho (fazer ainda)
	this->countA = 0;
	fprintf(fp, "Frame\tTime(s)\t");
	for (i = 0; i < this->count; i++) {
		if (this->ptTipo[i] == 1) // Ponto de trajetoria
		{
			fprintf(fp, "X%03d\tY%03d\t", CntPT, CntPT);
			CntPT++;
		} else
			this->countA++;
		if (this->countA == 3) // Atingiu o numero suficiente para calculo de um angulo
		{
			fprintf(fp, "Ang%03d\t", CntPA++);
			this->countA = 0;
		}
	}
	fprintf(fp, "\n");
	return fp;
}

///// NOVAS FUNCOES

bool cvMo::openVideo(char *fileName) {
	this->videoStream = cvCaptureFromAVI(fileName);
	if (!(this->videoStream)) {
		fprintf(stderr, "Could not initialize capturing...\n");
		cvReleaseCapture(&videoStream);
		return false;
	}
	capture = videoStream;
	screenName = fileName;
	cvNamedWindow(screenName, 0);

	cvSetMouseCallback(screenName, cvMo::on_mouseStatic, this);

	this->TotFrames = (int) cvGetCaptureProperty(this->capture,
				CV_CAP_PROP_FRAME_COUNT) - 1;
		this->FrameWidth = (int) cvGetCaptureProperty(this->capture,
				CV_CAP_PROP_FRAME_WIDTH);
		this->FrameHeight = (int) cvGetCaptureProperty(this->capture,
				CV_CAP_PROP_FRAME_HEIGHT);
		this->FrameRate
				= (int) cvGetCaptureProperty(this->capture, CV_CAP_PROP_FPS);
		cvResizeWindow(screenName, this->FrameWidth, this->FrameHeight);

		this->GridStepW = (int) (0.1 * this->FrameWidth);
		this->GridStepH = (int) (0.1 * this->FrameHeight);

		this->frame = cvQueryFrame(this->capture);

		cvInitFont(&(this->font), CV_FONT_VECTOR0, 0.8, 0.8, 0.0, 2, 8);

		/* allocate all the buffers */
		this->image = cvCreateImage(cvGetSize(this->frame), 8, 3);
		this->image->origin = this->frame->origin;
		this->grey = cvCreateImage(cvGetSize(this->frame), 8, 1);
		this->prev_grey = cvCreateImage(cvGetSize(this->frame), 8, 1);
		this->pyramid = cvCreateImage(cvGetSize(this->frame), 8, 1);
		this->prev_pyramid = cvCreateImage(cvGetSize(this->frame), 8, 1);
		this->points[0] = (CvPoint2D32f*) cvAlloc(MAX_COUNT
				* sizeof(this->points[0][0]));
		this->points[1] = (CvPoint2D32f*) cvAlloc(MAX_COUNT
				* sizeof(this->points[0][0]));
		this->ptTipo = (char*) malloc(MAX_COUNT * sizeof(char));

		this->status = (char*) cvAlloc(MAX_COUNT);
		this->flags = 0;
		this->FlisOpen = 1;

	Capture(1);
	LocatePoints();
	ShowImage();
	cvWaitKey(10);
	return true;
}

int cvMo::getTotalFrames(){
	return TotFrames;
}

}
