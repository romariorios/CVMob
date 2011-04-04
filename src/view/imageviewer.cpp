#include "imageviewer.h"

#include <vector>
using namespace std;

imageViewer::imageViewer()
{
    this->setGeometry(0,0,0,0);
    this->penFixPoint = QPen(QColor(0, 0, 255));
    this->penLineFixPoint = QPen(QColor(0, 0, 255));
    this->bruTextPanel = QBrush(QColor(0,0,255,100));
    this->penText = QPen(QColor(255, 0, 0));
    this->bruTrajPointPanel = QBrush(QColor(0,0,255,50));
    this->penTrajPointPanel = this->penTrajPoint = QPen(QColor(0,0,0));
    this->penTrajPointPanel.setStyle(Qt::DotLine);
    this->fontSize=10;
    this->fonText = QFont("Helvetica [Cronyx]",this->fontSize,-1,true);
    this->setMouseTracking(true);
}

void imageViewer::closeEvent(QCloseEvent *event){
    delete ProxyOpenCv::getInstance();
    emit closedImgView();
    event->accept();
}


void imageViewer::setImage(Mat image)
{
    this->image= QImage((uchar*)image.data,image.cols,image.rows,QImage::Format_RGB888);
}


void imageViewer::wheelEvent ( QWheelEvent * event )
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 10;
    int newValue=FacadeController::getInstance()->getWinSize()+numSteps;
    if(newValue<4)newValue=4;
    if (event->orientation() == Qt::Vertical)
        emit updatWinSize(newValue);
    event->accept();
}

void imageViewer::mouseMoveEvent(QMouseEvent *event)
 {
    this->mousePos=event->posF();
}

void imageViewer::mousePressEvent(QMouseEvent *event)
 {
    double px,py;
    int x=event->pos().x()+this->zoom.topLeft().x();
    int y=event->pos().y()+this->zoom.topLeft().y();

    int lxz=this->geometry().width();
    int lyz=this->geometry().height();

    px=(double)(x)/(lxz);
    py=(double)(y)/(lyz);
    x=px*ProxyOpenCv::getInstance()->image.cols;
    y=py*ProxyOpenCv::getInstance()->image.rows;

    //y = ProxyOpenCv::getInstance()->image->height - y;

    switch (event->button()) {
    case Qt::LeftButton:
        if(event->modifiers()==Qt::ControlModifier)
            ProxyOpenCv::getInstance()->captureVerticePoint(Point2f(x, y));
        else
            ProxyOpenCv::getInstance()->captureTrajectoriePoint(Point2f(x, y));
        break;
    case Qt::RightButton:
        ProxyOpenCv::getInstance()->captureFixedPoint(cvPointTo32f(cvPoint(x,y)));
        break;
    }
 }

void imageViewer::keyPressEvent(QKeyEvent *event)
{

    if(event->key()==Qt::Key_ZoomIn || event->key()==Qt::Key_Z)
    {
        zoom.setSize(QSizeF(this->zoom.size().width()-10,this->zoom.size().height()-10));
        zoom.moveCenter(this->mousePos);
    }
    if(event->key()==Qt::Key_ZoomOut || event->key()==Qt::Key_X)
    {
        zoom.setSize(QSizeF(this->zoom.size().width()+10,this->zoom.size().height()+10));
        zoom.moveCenter(this->mousePos);
    }
    int mxW=this->geometry().width();
    int myW=this->geometry().height();
    if(zoom.topLeft().x()<0) zoom.setTopLeft(QPointF(0,zoom.topLeft().y()));
    if(zoom.topLeft().y()<0) zoom.setTopLeft(QPointF(zoom.topLeft().x(),0));
    if(zoom.bottomRight().x()>mxW) zoom.setBottomRight(QPointF(mxW,zoom.bottomRight().y()));
    if(zoom.bottomRight().y()>myW) zoom.setBottomRight(QPointF(zoom.bottomRight().x(),myW));
//    qDebug() << zoom.bottomRight().x() <<","<< zoom.bottomRight().y();
//    qDebug() << mxW <<"/"<< myW;
    this->repaint();

    emit updateKeyboard(event->key());
    QWidget::keyPressEvent(event);
    return;
}




void imageViewer::paintEvent(QPaintEvent* event)
{
    double fx,fy;
    QPainter estojo;
    ProxyOpenCv *cV;
    cV=ProxyOpenCv::getInstance();

    // desenhar na imagem e plotar a imagem
    int lxz=this->geometry().width();
    int lyz=this->geometry().height();

    fx=(double)lxz/cV->image.cols;
    fy=(double)lyz/cV->image.rows;

    estojo.begin(this);
    // Draw Captured Frame
    QRectF target(0.0, 0.0, this->geometry().width(),this->geometry().height());
    if(zoom.isNull())
        zoom= QRectF(0.0, 0.0, this->geometry().width(),this->geometry().height());

    estojo.drawImage(target,this->image,zoom);

    // Draw GUI Points
    this->drawFixedPoints(&estojo,cV,fx,fy);
    this->drawTrajPoints(&estojo,cV,fx,fy);
    this->drawAnglePoints(&estojo,cV,fx,fy);

    estojo.end();
}

void imageViewer::drawTrajPoints(QPainter *estojo,ProxyOpenCv *cV,double fx,double fy)
{
    int xVect,yVect,countTraj;
    QPointF pA,pA2;
    QPen cane;
    // for each point marked on screen
    // draw your trajectorie
    for (int i = 0; i < cV->get_countPoints(); i++) {
        countTraj=cV->points[i].time.size();
        pA.setX(cV->points[i].lastPoint.x*fx);
        pA.setY(cV->points[i].lastPoint.y*fy);
        estojo->setPen(this->penTrajPoint);
        estojo->setBrush(QColor(cV->points[i].color[0],cV->points[i].color[1],cV->points[i].color[2]));
        estojo->drawEllipse(pA,3,3);
        pA.setX(pA.x()-cV->winSize*fx);
        pA.setY(pA.y()-cV->winSize*fy);
        estojo->setPen(this->penTrajPointPanel);
        estojo->setBrush(this->bruTrajPointPanel);
        estojo->drawRect(pA.x(),pA.y(),2*cV->winSize*fx,2*cV->winSize*fy);


        for (int j = 0; j < countTraj; j++)
        {
                    pA.setX(cV->points[i].trajectorie[j].x*fx-1);
                    pA.setY(cV->points[i].trajectorie[j].y*fy-1);
                    pA2=pA;
                    // Draw Point
                    estojo->setPen(this->penTrajPoint);
                    estojo->setBrush(QColor(cV->points[i].color[0],cV->points[i].color[1],cV->points[i].color[2]));
                    estojo->drawEllipse(pA,3,3);
                    // Draw Vectors
                    if(cV->showVectors){
                        if(cV->vectorsType==1 && j>1 &&j<countTraj-2){
                            xVect=(double)cV->points[i].xVelocity[j]/cV->points[i].velocity[j]*25;
                            yVect=(double)cV->points[i].yVelocity[j]/cV->points[i].velocity[j]*25;
                        }
                        if(cV->vectorsType==2 && j>3 && j<countTraj-3){
                            xVect=(double)cV->points[i].xAcceleration[j]/cV->points[i].acceleration[j]*25;
                            yVect=(double)cV->points[i].yAcceleration[j]/cV->points[i].acceleration[j]*25;
                        }

                        pA2.setX(pA.x()+xVect);
                        pA2.setY(pA.y()+yVect);

                        if( (cV->vectorsType==1 && j>1 && j<countTraj-2)
                            ||
                            (cV->vectorsType==2 && j>3 && j<countTraj-3))
                        {
                            cane.setWidth(3);
                            cane.setColor(QColor(cV->points[i].color[0],cV->points[i].color[1],cV->points[i].color[2],180));
                            estojo->setPen(cane);
                            estojo->drawLine(pA,pA2);
                        }
                    }
        }
    }
}

void imageViewer::drawFixedPoints(QPainter *estojo,ProxyOpenCv *cV,double fx,double fy)
{
    // draw fixed points
    QPointF p1,p2,p3,p4,p,pa,pt;


    double d;
    int sz=10;


    for (unsigned int i = 0; i < cV->fixedPoints.size(); i++) {
        p.setX(cV->fixedPoints[i].markedPoint.x * fx);
        p.setY(cV->fixedPoints[i].markedPoint.y * fy);

        p1.setX(p.x());
        p1.setY(p.y()-sz);

        p2.setX(p.x());
        p2.setY(p.y()+sz);

        p3.setX(p.x()-sz);
        p3.setY(p.y());

        p4.setX(p.x()+sz);
        p4.setY(p.y());

        estojo->setPen(this->penFixPoint);
        estojo->drawLine(p1,p2);
        estojo->drawLine(p3,p4);
        if(i%2)
        {
            estojo->setPen(this->penLineFixPoint);
            estojo->drawLine(p,pa);
            d=this->calcDistance(cV->fixedPoints[i].markedPoint,
                                 cV->fixedPoints[i-1].markedPoint,
                                 cV->get_horizontalRazao(),cV->get_verticalRazao());
            // Draw distance text
            pt.setX(p.x()-(p.x()-pa.x())/2);
            pt.setY(p.y()-(p.y()-pa.y())/2);
            estojo->setBrush(this->bruTextPanel);
            estojo->drawRect(pt.x(),pt.y()-this->fontSize,this->fontSize*(4+this->op.prec),1.5*this->fontSize);
            estojo->setPen(this->penText);
            estojo->drawText(QRect(pt.x(),pt.y()-this->fontSize,this->fontSize*(4+this->op.prec),1.5*this->fontSize),
                             Qt::AlignHCenter, "P"+QString::number(i/2+1)+ ":"+QString::number(d,'f',this->op.prec));

        }
        pa=p;
    }

}

void imageViewer::drawAnglePoints (QPainter *estojo,ProxyOpenCv *cV,double fx,double fy)
{
    QPointF p,pA;
    QPen cane;

    // for each point marked on screen
    // draw your vertices
    estojo->setFont(this->fonText);

    for (int i = 0; i < cV->angles.size(); i++)
    {
        for(int j=0;j< cV->angles[i].vertices.size();j++)
        {
            p.setX(cV->angles[i].vertices[j].x*fx);
            p.setY(cV->angles[i].vertices[j].y*fy);
            estojo->setPen(this->penTrajPoint);
            estojo->setBrush(QColor(cV->angles[i].color[0],cV->angles[i].color[1],cV->angles[i].color[2]));
            estojo->drawEllipse(p,3,3);
            if(j>0)         // Draw angle arc
            {
                pA.setX(cV->angles[i].vertices[j-1].x*fx);
                pA.setY(cV->angles[i].vertices[j-1].y*fy);
                cane.setWidth(3);
                cane.setColor(QColor(0,255,0,180));
                estojo->setPen(cane);
                estojo->drawLine(p,pA);
            }
            p.setX(p.x()-cV->winSize*fx);
            p.setY(p.y()-cV->winSize*fy);
            estojo->setPen(this->penTrajPointPanel);
            estojo->setBrush(this->bruTrajPointPanel);
            estojo->drawRect(p.x(),p.y(),2*cV->winSize*fx,2*cV->winSize*fy);
        }
        // draw angle elipse
        if(cV->angles[i].vertices.size()==3)
        {
            p.setX(cV->angles[i].vertices[1].x*fx-cV->winSize*fx);
            p.setY(cV->angles[i].vertices[1].y*fy-cV->winSize*fx);
            estojo->setBrush(QBrush(QColor(255,255,255,100)));
            estojo->setPen(this->penTrajPointPanel);
            estojo->drawRect(p.x(),p.y()-this->fontSize,this->fontSize*(4+this->op.prec),1.5*this->fontSize);
            estojo->setPen(QPen(QColor(255,0,0,255)));
            estojo->setBrush(this->bruTrajPointPanel);
            estojo->drawText(QRect(p.x(),p.y()-this->fontSize,this->fontSize*(4+this->op.prec),1.5*this->fontSize),
                             Qt::AlignHCenter,
                             QString::number(cV->angles[i].value[cV->angles[i].value.size()-1])+ "º");

//            estojo->drawPie((int)p.x(),(int)p.y(),
//                            (int)(2*cV->winSize*fx),
//                            (int)(2*cV->winSize*fy),
//                            (int)cV->angles[i].ang1*16,
//                            (int)cV->angles[i].ang2*16);
        }
    }
}


double imageViewer::calcDistance(CvPoint2D32f p1,CvPoint2D32f p2,float hR,float vR)
{
    double dx=(p2.x-p1.x);
    double dy=(p2.y-p1.y);
    return sqrt(dx*dx*hR*hR+dy*dy*vR*vR);
}
