#ifndef SATELLITELINK_H
#define SATELLITELINK_H

#include <QUdpSocket>
#include <QTcpSocket>
#include <QQueue>
#include <QSet>
#include <QTimer>
#include "payload.h"

class SatelliteLink : public QObject
{
    Q_OBJECT

    QHostAddress localAddress;
    QHostAddress remoteAddress;
    quint16 port;
    QUdpSocket socket;
   // QTcpSocket socket;
    bool bound;
    bool checkChecksum;
    QSet<quint32> topics;
    qint64 receivedBytes;
    qint64 sentBytes;
    QTimer timer;
signals:
    void readReady();

public:
    explicit SatelliteLink(QObject *parent = 0, bool checkChecksum = true);
    void addTopic(PayloadType);
    int write(quint32 topicId, const QByteArray &data);
    //int write(quint32 topicId, const Telecommand &telecommand);
    template<class T>
    int write (quint32 topicId, const T &data)
    {
        QByteArray buffer (sizeof (T), 0x00);
        memcpy (buffer.data(), (char*)&data, sizeof(T));
        return write(topicId, buffer);
    }
    Payload read();
    qint64 readAndResetReceivedBytes();
    qint64 readAndResetSentBytes();
    bool isBound();
};

#endif // SATELLITELINK_H
