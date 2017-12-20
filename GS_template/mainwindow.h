#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "satellitelink.h"
#include "manualcontrol.h"

extern bool graphvalue;
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
    SatelliteLink *link2;

private:
    Ui::MainWindow *ui;
    ManualControl *mcWindow;
signals:
    void PacketSignal(void);
public slots:
void readFromLink();
void readFromGSLink();
void sendtelecommand();
void setupGraph();
void setupGraphrealtimeDataSlot(void);
void setSignal(QColor color);
void on_pushButton_clicked(void);
private slots:
void on_pbManualControl_clicked();
};

#endif // MAINWINDOW_H
