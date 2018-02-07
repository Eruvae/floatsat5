#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCore/QDebug>
#include "chartview.h"




void MainWindow::SetupRadar()
{



//Start Radar
const qreal angularMin = 0;
const qreal angularMax = 360;

const qreal radialMin = 0;
const qreal radialMax = 1;



 series1 = new QScatterSeries();
 series1->setColor(QColor(0, 153, 51));
 series1->setBorderColor(QColor(0, 153, 51));
 series1->setMarkerSize(11);
 series1->append(80, 90); //to plot
 series1->clear(); //to plot


chart = new QPolarChart();
chart->addSeries(series1); //to plot
chart->setBackgroundVisible(false);


angularAxis = new QValueAxis();
angularAxis->setTickCount(5); // First and last ticks are co-located on 0/360 angle.
angularAxis->setLabelsColor(Qt::white);
angularAxis->setLabelFormat("%.1f");
angularAxis->setShadesVisible(false);
angularAxis->setShadesBrush(QBrush(QColor(0, 0, 0)));
chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

radialAxis = new QValueAxis();
radialAxis->setTickCount(5);
radialAxis->setLabelFormat("%d");
radialAxis->setLabelsColor(Qt::white);
chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

   series1->attachAxis(radialAxis);
   series1->attachAxis(angularAxis);

radialAxis->setRange(radialMin, radialMax);
angularAxis->setRange(angularMin, angularMax);

ui->pchart->setChart(chart); //to plot
ui->pchart->setRenderHint(QPainter::Antialiasing);
//End Radar

}
