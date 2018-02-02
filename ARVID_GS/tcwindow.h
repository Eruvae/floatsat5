#ifndef TCWINDOW_H
#define TCWINDOW_H
#include "satellitelink.h"

#include <QDialog>

namespace Ui {
class TCWindow;
}

class TCWindow : public QDialog
{
    Q_OBJECT

public:
    /*explicit */TCWindow(QWidget *parent = 0, SatelliteLink *link = 0);
    ~TCWindow();
    SatelliteLink *TCLink;

private:
    Ui::TCWindow *ui;


public slots:
    void sendtelecommand();


private slots:
    void on_pb_clicked();
};


#endif // TCWINDOW_H
