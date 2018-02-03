#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "satellitelink.h"
#include "tcwindow.h"
#include "manualcontrol.h"


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
    //SatelliteLink *gsLink;

private:
    Ui::MainWindow *ui;
    TCWindow *tcwindow;
    ManualControl *manualControl;


signals:
    void PacketSignal(double newValue);
    void powerDataUpdate();

public slots:
void readFromLink();
void sendtelecommand();
void SetupGraphCurrent();
void SetupPlotTracking(float valuex, float valuey);
//void SetupRWSpeedMeter();
void SetupRealtimeDataSlotCurrent(double newValue);
//void setSignal(QColor color);


private slots:
void on_actionTelecommand_Interface_triggered();
void on_actionManual_Control_triggered();
void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
