#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

void MainWindow::SetupPlotTracking(float valuex, float valuey)
{
    QCPScatterStyle shape;
    shape .setShape(QCPScatterStyle::ssTriangle);
    shape .setPen(QPen(Qt::red));
    shape .setSize(8);

    ui->trackPlot->addGraph();
    ui->trackPlot->graph(0)->setScatterStyle(shape);
    ui->trackPlot->setBackground(Qt::white);
    ui->trackPlot->axisRect()->setupFullAxesBox();
    ui->trackPlot->xAxis->setAutoTickStep(false);
    ui->trackPlot->xAxis->setTickStep(0.01);
    ui->trackPlot->xAxis->setRange(0, 2);
    ui->trackPlot->yAxis->setAutoTickStep(false);
    ui->trackPlot->yAxis->setTickStep(0.01);
    ui->trackPlot->yAxis->setRange(0, 2);
    ui->trackPlot->xAxis->setRangeReversed(true);
    ui->trackPlot->graph(0)->addData(valuex,valuey);

}
