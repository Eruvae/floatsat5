#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "satellitelink.h"
#include "tcwindow.h"


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
signals:
    void PacketSignal(double newValue);
public slots:
void readFromLink();
void sendtelecommand();
void SetupGraphCurrent();
void SetupCompass();
//void SetupRWSpeedMeter();
void SetupRealtimeDataSlotCurrent(double newValue);
//void setSignal(QColor color);


private slots:
void on_actionTelecommand_Interface_triggered();
};

#endif // MAINWINDOW_H
