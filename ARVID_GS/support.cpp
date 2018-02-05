#include "support.h"
#include "ui_support.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QThread>

#define CALIB_IMU 0x00
#define SEND_POSE 0x01
#define SEND_RW_SPEED 0x02
#define THRUSTER_CONTROL 0x03
#define DEACTIVATE_CONTROLLER 0x04
#define CHANGE_PC_MODE	0x05
#define RPI_COMMAND	0x06
#define SEND_CONTROL_PARAMS 0x07
#define SEND_ROTATION_SPEED 0x08
#define SEND_POSE_TO_LIST 0x09

Support::Support(QWidget *parent, SatelliteLink *link) :
    QDialog(parent),
    ui(new Ui::Support),
    TCLink(link)
{
    ui->setupUi(this);
}

void Support::telecommand()
{
    Telecommand sender;

}

Support::~Support()
{
    delete ui;
}
