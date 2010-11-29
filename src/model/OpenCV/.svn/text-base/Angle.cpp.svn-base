
#include "Angle.h"
#include <math.h>



void Angle::calcValue(double ticTac)
{
    if(this->vertices.size()<3)
       return;
    double dx,dy;
    double AngR, AngG1,AngG2;
    dx=this->vertices[1].x-this->vertices[0].x;
    dy=this->vertices[1].y-this->vertices[0].y;
    if(dx==0 && dy==0) { return;}
    AngR=atan2(dy,dx);
    AngG1=(360.0/(2.0*3.1416))*AngR;
    dx=this->vertices[1].x-this->vertices[2].x;
    dy=this->vertices[1].y-this->vertices[2].y;
    if(dx==0 && dy==0) { return;}
    AngR=atan2(dy,dx);
    AngG2=(360.0/(2.0*3.1416))*AngR;
    this->value.push_back(360-fabs(AngG1-AngG2));
    this->time.push_back(ticTac);
    this->ang1=AngG1;
    this->ang2=AngG2;
    return;


}
