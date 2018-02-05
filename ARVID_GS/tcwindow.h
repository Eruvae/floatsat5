#ifndef TCWINDOW_H
#define TCWINDOW_H
#include "satellitelink.h"

#include <QDialog>
#include <QFile>

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
    QVector<Pose2D> poses;
    void readPosesFromFile(QFile &file, QVector<Pose2D> &poses);


public slots:
    void sendtelecommand();
    void telecommandsend();

private slots:
    void on_pb_clicked();
    void on_pushButton_clicked();
    void on_pushButton_4_clicked();
    void on_readPoseFileButton_clicked();
};


#endif // TCWINDOW_H
