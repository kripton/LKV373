#include "datareceiver.h"

dataReceiver::dataReceiver(QObject *parent) :
    QObject(parent)
{
    run = false;
    sock = 0;
}

bool dataReceiver::init(QHostAddress sender)
{
    this->sender = sender;
    sock = socket(AF_PACKET, SOCK_RAW, ntohs(3));
    if (sock > 0)
    {
        return true;
    }
    return false;
}

void dataReceiver::startReceive()
{
    bool frameValid = false; // First frame to be considered invalid

    qDebug() << "DataReceiver starting";

    if (sock == 0)
    {
        qCritical() << "Socket not existing - call init first!";
        return;
    }

    expectedChunk = 0;
    lastFrameNo = 0;

    run = true;
    while (run)
    {
        QByteArray array(1200, 0);
        recv(sock, array.data(), 1200, 0);

        if (array.data()[23] != 0x11) continue; // Not UDP

        quint64* ethSourceAndProto = (quint64*)(array.data() + 6);
        if (*ethSourceAndProto != 0x0008016000780b00) continue; // Source MAC or Proto (= IP) didn't match

        quint32* source = (quint32*)(array.data() + 26);
        *source = ntohl(*source);
        if (*source != sender.toIPv4Address()) continue; // Source IP didn't match

        quint32* dest = (quint32*)(array.data() + 30);
        if (*dest != 0x020202e2) continue; // Dest IP (= "226.2.2.2") didn't match

        quint32* ports = (quint32*)(array.data() + 34);
        if (*ports == 0x14081408)
        {
            // Video data

            // Payload starts after all headers, length is taken from UDP header. Length in IP header is wrong!
            char* dataPtr = array.data() + 42;
            quint16 length = array.data()[38] * 256 + array.data()[39];
            QByteArray data(dataPtr, length);

            quint16 frameNo = (quint16)data.data()[0] * 256 + (quint8)data.data()[1];
            quint16 chunkNo = (quint16)data.data()[2] * 256 + (quint8)data.data()[3];
            bool lastChunk = false;
            if (chunkNo & 0x8000)
            {
                lastChunk = true;
            }

            if ((!lastChunk) && (chunkNo != expectedChunk))
            {
                qWarning() << "Possible packet loss. Expected chunk" << expectedChunk << "got" << chunkNo;
                frameValid = false;
            }
            expectedChunk++;

            frame.append(data.data() + 4, length - 4); // Skip the first 4 byte (frame and chunk counter)
            if (lastChunk)
            {
                //QImage image = QImage::fromData(frame);
                //image.save(QString("test_%1.jpg").arg(frameNo));

                if (frameNo != (lastFrameNo + 1))
                {
                    qWarning() << "Possible packet loss. Expected frame" << (lastFrameNo + 1) << "got" << frameNo;
                }
                lastFrameNo = frameNo;

                //qDebug() << "DATARECEIVER GOT FRAME" << frameNo;

                emit newVideoFrame(frame, frameValid);
                frameValid = true;
                frame.clear();
                expectedChunk = 0;
            }
        }
        else if (*ports == 0x12081208)
        {
            // Audio data

            // Payload starts after all headers; 16 bytes are skipped
            char* dataPtr = array.data() + 42 + 16;
            quint16 length = array.data()[38] * 256 + array.data()[39] - 16;
            QByteArray data(dataPtr, 1000); // TODO: Use length from UDP header? Or from IP header? Or Fixed size?
            emit newAudioChunk(data);
        }
    }
}

void dataReceiver::stopReceive()
{
    qDebug() << "DataReceiver stopping";
    run = false;
}
