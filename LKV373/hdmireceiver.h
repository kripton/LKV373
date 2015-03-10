#ifndef HDMIRECEIVER_H
#define HDMIRECEIVER_H

#include <QDebug>
#include <QObject>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QThread>
#include <QPixmap>

#include "datareceiver.h"

class Q_DECL_EXPORT HdmiReceiver : public QObject
{
    Q_OBJECT
public:
    explicit HdmiReceiver(QObject *parent = 0, QHostAddress sender_address = QHostAddress::Any);
    ~HdmiReceiver();

    bool parseFrames;
    bool emitInvalidFrames;

private:
    QHostAddress sender_address;
    QUdpSocket* udpControlDataSocket;

    QThread receiverThread;
    dataReceiver* receiver;

signals:
    void startDataReceiver();
    void stopDataReceiver();

    void controlPacketReceived(QHostAddress sender, bool link, quint16 width, quint16 height, qreal fps);
    void newVideoFrame(QByteArray frameData);
    void newAudioChunk(QByteArray audioData);

public slots:
    void readControlDatagram();

private slots:
    void recvNewVideoFrame(QByteArray frameData, bool frameValid);
    void recvAudioChunk(QByteArray audioData);
};

#endif // HDMIRECEIVER_H
