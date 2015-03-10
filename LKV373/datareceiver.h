#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include <QDebug>
#include <QObject>
#include <QHostAddress>

#include <sys/socket.h>
#include <netinet/in.h>

class dataReceiver : public QObject
{
    Q_OBJECT
public:
    explicit dataReceiver(QObject *parent = 0);
    bool init(QHostAddress sender);

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
