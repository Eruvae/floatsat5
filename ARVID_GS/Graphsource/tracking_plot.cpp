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
    shape.setPen(QPen(Qt::blue));
    shape.setSize(8);

    ui->trackPlot->setBackground(Qt::black);
    ui->trackPlot->yAxis->setTickLabelColor(Qt::white);
    ui->trackPlot->xAxis->setTickLabelColor(Qt::white);
    ui->trackPlot->addGraph();
    ui->trackPlot->addGraph();

    ui->trackPlot->graph(0)->setScatterStyle(shape);
    ui->trackPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->trackPlot->graph(1)->setPen(QPen(QColor(255, 0, 0)));
    ui->trackPlot->xAxis->setLabel("Y");
    ui->trackPlot->yAxis->setLabel("X");
    ui->trackPlot->axisRect()->setupFullAxesBox();
    ui->trackPlot->xAxis->setAutoTickStep(false);
    ui->trackPlot->xAxis->setTickStep(0.5);
    ui->trackPlot->xAxis->setRange(0, 2.2);
    ui->trackPlot->yAxis->setAutoTickStep(false);
    ui->trackPlot->yAxis->setTickStep(0.5);
    ui->trackPlot->yAxis->setRange(0, 2.2);

}
