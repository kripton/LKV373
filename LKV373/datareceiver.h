#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include <QDebug>
#include <QObject>
#include <QHostAddress>

#include <sys/socket.h>
#include <netinet/in.h>

//! Worker class that receives RAW packets, filters them and
//! emits video frames and audio chunks as QByteArray.
//! Needs to run in an separate thread since it loops endlessly.
class dataReceiver : public QObject
{
    Q_OBJECT
public:
    explicit dataReceiver(QObject *parent = 0);
    bool init(QHostAddress sender);
    void changeSender(QHostAddress sender);

private:
    bool run;

    QHostAddress sender;
    int sock;

    QByteArray frame;
    quint16 expectedChunk;
    quint16 lastFrameNo;

signals:
    void newVideoFrame(QByteArray frameData, bool frameValid);
    void newAudioChunk(QByteArray audioData);

public slots:
    void startReceive();
    void stopReceive();
};

#endif // DATARECEIVER_H
