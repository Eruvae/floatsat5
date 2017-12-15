#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "satellitelink.h"


//extern double graphvaluecurrent;
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
    SatelliteLink *gsLink;

private:
    Ui::MainWindow *ui;
signals:
    void PacketSignal(double newValue);
public slots:
void readFromLink();
void sendtelecommand();
void SetupGraphCurrent();
void SetupRWSpeedMeter();
void SetupRealtimeDataSlotCurrent(double newValue);
void setSignal(QColor color);
void on_pushButton_clicked(void);

};

#endif // MAINWINDOW_H
