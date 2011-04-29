/*
 * Plot.h
 * Represents a qwt graph.
 *  Created on: 16/08/2008
 *      Author: paulista
 */

#ifndef PLOT_H_
#define PLOT_H_
//#define QWT_DLL
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_symbol.h>
#include <qlist.h>

#include "cvZoomer.h"
#include <vector>

using namespace std;

class Plot : public QwtPlot{
public:
	Plot(QString Title, QString AxisXTitle, QString AxisYTitle);
	void addCurve(int index, int r, int g, int b);
        void setData(int curve, QVector<double> xData, QVector<double> yData);
	void releaseCurves();
	void setXAxisTitle(QString title);
	void setYAxisTitle(QString title);
	virtual ~Plot();

private:
	QList<QwtPlotCurve*> curves;
	// used to show on the mouse the x,y values
	QwtPlotZoomer* zoomer;

};

#endif /* PLOT_H_ */
