/*
 * Plot.cpp
 *
 *  Created on: 16/08/2008
 *      Author: paulista
 */

#include "Plot.h"
#include <iostream>
#include <qnamespace.h>
#include <QMessageBox>
#include <qwt/qwt.h>

#define MSGN(x) QMessageBox *message = new QMessageBox();message->setText(QString::number(x));int _r=message->exec()
#define MSGN2(x) message->setText(QString::number(x)); _r=message->exec()


using namespace std;
namespace view {

//TODO : change X title with the constructor
/**
 * create a graph with Title = Title , X Axis = AxisXTitle and Y Axis = AxisYTitle
 */

Plot::Plot(QString Title, QString AxisXTitle, QString AxisYTitle) {
	setTitle(Title);
//	insertLegend(new QwtLegend(), QwtPlot::RightLegend);
	// Set axis titles]

	/*
	 * teste para mudar o fundo do grafico
	 */
//	QPalette palette;
//	 palette.setColor(QPalette::Background, Qt::black);
//	 this->setPalette(palette);

	setAxisTitle(xBottom, AxisXTitle);
	setAxisTitle(yLeft, AxisYTitle);
        zoomer = new cvZoomer(this->canvas());
}

void Plot::addCurve(int index, int r, int g, int b){
	QwtPlotCurve* curve = new QwtPlotCurve();
	curve->setStyle(QwtPlotCurve::Lines);
        QwtSymbol *sym = new QwtSymbol;
        sym->setStyle(QwtSymbol::Cross);
        sym->setPen(QColor::fromRgb(r,g,b));
        sym->setSize(5);
	curve->setSymbol(sym);

#if QT_VERSION >= 0x040000
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
#endif
	curve->setPen(QPen(QColor::fromRgb(r,g,b)));
	curve->attach(this);
	curves.append(curve);
}


void Plot::setData(int curve, QVector<double> xData, QVector<double> yData){

    curves.at(curve)->setSamples(xData, yData);
}

void Plot::releaseCurves(){
	foreach(QwtPlotCurve* curve , curves)
		curve->detach();
	curves.clear();
}

void Plot::setXAxisTitle(QString title){
	setAxisTitle(xBottom, title);
}

void Plot::setYAxisTitle(QString title){
	setAxisTitle(yLeft, title);
}

Plot::~Plot() {
	// TODO Auto-generated destructor stub
}

}
