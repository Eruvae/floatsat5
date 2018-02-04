#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

void MainWindow::SetupPlotTracking()
{
    QCPScatterStyle shape;
    shape.setShape(QCPScatterStyle::ssTriangle);
    shape.setPen(QPen(Qt::red));
    shape.setSize(8);

    ui->trackPlot->addGraph();
    ui->trackPlot->graph(0)->setScatterStyle(shape);
    ui->trackPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->trackPlot->axisRect()->setupFullAxesBox();
    ui->trackPlot->xAxis->setAutoTickStep(false);
    ui->trackPlot->xAxis->setTickStep(0.5);
    ui->trackPlot->xAxis->setRange(0, -2.2);
    ui->trackPlot->xAxis->setRangeReversed(true);
    ui->trackPlot->yAxis->setAutoTickStep(false);
    ui->trackPlot->yAxis->setTickStep(0.5);
    ui->trackPlot->yAxis->setRange(0, 2.2);


    QTimer *dataTimer = new QTimer();

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:

    dataTimer->start(500);
    connect(dataTimer, SIGNAL(timeout()),ui->trackPlot, SLOT(replot()));

}
