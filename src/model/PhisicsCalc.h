/*
 * PhisicsCalc.h
 *
 *  Created on: 24/09/2008
 *      Author: paulista
 */

#ifndef PHISICSCALC_H_
#define PHISICSCALC_H_

namespace model {

class PhisicsCalc {

private:

public:
	PhisicsCalc();
	double calculateAxisVelocity(double S1, double S2, double time);
	double calculateAxisAcceleration(double S1, double S2, double S3, double S4, double time);
	double calculateVelocity(double Vx, double Vy);
	double calculateAcceleration(double Ax, double Ay);
	double calculateWork(double dx, double dy, double acceleration);
	virtual ~PhisicsCalc();
};

}

#endif /* PHISICSCALC_H_ */
