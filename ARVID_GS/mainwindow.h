#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "satellitelink.h"
#include "satellitelink2.h"

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
    Satellitelink2 *link;

private:
    Ui::MainWindow *ui;
signals:
    void PacketSignal(void);
public slots:
void readFromLink();
void sendtelecommand();
void setupGraph();
void setupGraphrealtimeDataSlot(void);
void setSignal(QColor color);
void on_pushButton_clicked(void);

};

#endif // MAINWINDOW_H
