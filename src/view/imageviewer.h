#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QtGui>
#include <QWidget>
#include <opencv/cv.h>
#include "options.h"

#include  "../model/OpenCV/ProxyOpenCv.h"
#include "../controller/FacadeController.h"

using namespace model;
using namespace controller;


class imageViewer : public QWidget
{
    Q_OBJECT
public:
    imageViewer();
    QGridLayout imageLayout;
    QLabel imageLabel;
    void drawImage(Mat image);
    void setImage(Mat image);
    options op;                         // CvMob view Options
private:
    QImage image;
    void drawFixedPoints(QPainter *estojo,ProxyOpenCv *cV,double fx,double fy);
    void drawTrajPoints(QPainter *estojo,ProxyOpenCv *cV,double fx,double fy);
    void drawAnglePoints(QPainter *estojo,ProxyOpenCv *cV,double fx,double fy);

    double calcDistance(CvPoint2D32f p1,CvPoint2D32f p2,float hR,float vR);
    QPen penFixPoint;
    QPen penLineFixPoint;
    QBrush bruTextPanel;
    QBrush bruTrajPointPanel;
    QPen penTrajPointPanel;
    QPen penTrajPoint;
    QPen penText;
    QFont fonText;
    QRectF zoom;
    int fontSize;
    QPointF mousePos;


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent* event);
    void wheelEvent ( QWheelEvent * event );


signals:
    void updateKeyboard(int c);
    void updatWinSize(double value);
    void closedImgView();

};

#endif // IMAGEVIEWER_H
