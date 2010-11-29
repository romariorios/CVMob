/*
 * PhisicsCalc.cpp
 *
 *  Created on: 24/09/2008
 *      Author: paulista
 */

#include "PhisicsCalc.h"
#include<iostream>
#include<math.h>
using namespace std;
namespace model {

PhisicsCalc::PhisicsCalc() {
	// TODO Auto-generated constructor stub

}

/**
 * usa duas variações de espaço em um eixo para calcular a velocidade no eixo.
 * S1 = variação do frame -1 até o frame
 * S2 = variação do frame até o frame +1
 */
double PhisicsCalc::calculateAxisVelocity(double S1, double S2, double time){
        return (S1 + S2)/ (2*time);
}


/**
 * usa quatro variações de espaço em um eixo para calcular a aceleração no eixo.
 * S1= variação do frame até o frame +1
 * S2= variação do frame+1 até o frame+2
 * S3= variação do frame -2 até o frame -1
 * S4= variação do frame-1 até o frame
 * TODO: COLOCAR PARENTESES NO 4*...
 */
double PhisicsCalc::calculateAxisAcceleration(double S1, double S2, double S3, double S4, double time){
        return (S1 + S2 - S3 - S4 )/ (4*(time*time));
}

/**
 *
 */

double PhisicsCalc::calculateVelocity(double Vx, double Vy) {
	double v = sqrt(pow(Vx,2) + pow(Vy ,2));
	return v;
}

double PhisicsCalc::calculateAcceleration(double Ax, double Ay) {
	double a = sqrt(pow(Ax,2) + pow(Ay ,2));
	return a;
}


double PhisicsCalc::calculateWork(double dx, double dy, double acceleration) {
	double w = (sqrt(pow(dx,2) + pow(dy ,2))) * acceleration;
	return w;
}

PhisicsCalc::~PhisicsCalc() {
	// TODO Auto-generated destructor stub
}

}
