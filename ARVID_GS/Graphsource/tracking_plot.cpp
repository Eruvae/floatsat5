#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

void MainWindow::SetupPlotTracking()
{

    ui->trackPlot->setBackground(Qt::black);
    ui->trackPlot->yAxis->setTickLabelColor(Qt::white);
    ui->trackPlot->xAxis->setTickLabelColor(Qt::white);

    ui->trackPlot->addGraph(); //Object Tracking Position

    QCPScatterStyle otshape;
    otshape.setShape(QCPScatterStyle::ssDiamond);
    otshape.setPen(QPen(Qt::green));


    ui->trackPlot->graph(0)->setScatterStyle(otshape);
    ui->trackPlot->graph(0)->setLineStyle(QCPGraph::lsNone);


    ui->trackPlot->xAxis->setLabel("Y");
    ui->trackPlot->xAxis->setLabelColor(QColor(255, 255, 255));
    ui->trackPlot->yAxis->setLabel("X");
    ui->trackPlot->yAxis->setLabelColor(QColor(255, 255, 255));
    ui->trackPlot->axisRect()->setupFullAxesBox();
    ui->trackPlot->xAxis->setAutoTickStep(false);
    ui->trackPlot->xAxis->setTickStep(0.5);
    ui->trackPlot->xAxis->setRange(0, 2.2);
    ui->trackPlot->yAxis->setAutoTickStep(false);
    ui->trackPlot->yAxis->setTickStep(0.5);
    ui->trackPlot->yAxis->setRange(0, 2.2);

    QCPScatterStyle shape;
    shape.setShape(QCPScatterStyle::ssTriangle);
    shape.setPen(QPen(Qt::blue));
    shape.setSize(8);



    track = new QCPCurve(ui->trackPlot->xAxis, ui->trackPlot->yAxis);
    track->setScatterStyle(shape);
    track->setLineStyle(QCPCurve::LineStyle::lsNone);
    track->setPen(QPen(Qt::blue));

    trackline = new QCPCurve(ui->trackPlot->xAxis, ui->trackPlot->yAxis);
    trackline->setPen(QPen(Qt::red));




}
