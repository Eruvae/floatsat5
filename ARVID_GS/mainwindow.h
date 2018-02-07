#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScatterSeries>
#include <QPolarChart>
#include <QValueAxis>
#include "satellitelink.h"
#include "tcwindow.h"
#include "manualcontrol.h"
#include "support.h"
#include "qcustomplot.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    SatelliteLink *link;
   

private:
    Ui::MainWindow *ui;
    TCWindow *tcwindow;
    ManualControl *manualControl;
    Support *support;    

    QQueue<int> batteryStateQueue;

    QCPCurve *track;
    QCPCurve *trackline;



signals:
    void PacketSignal(double newValue);
    void powerDataUpdate();

public slots:
void readFromLink();
void SetupGraphCurrent();
void SetupPlotTracking();
void SetupRealtimeDataSlotCurrent(double newValue);
void SetupSpeedMeter();



private slots:
void on_actionTelecommand_Interface_triggered();
void on_actionManual_Control_triggered();
void on_pushButton_clicked();
void on_actionSupport_and_Maintainance_triggered();
};

#endif // MAINWINDOW_H
