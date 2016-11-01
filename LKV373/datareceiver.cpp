#include "datareceiver.h"

dataReceiver::dataReceiver(QObject *parent) :
    QObject(parent)
{
    run = false;
    sock = 0;
    ifindex = 0xFFFF;
    addr_len = sizeof(src_addr);
}

bool dataReceiver::init(QHostAddress sender)
{
    this->sender = sender;
    sock = socket(AF_PACKET, SOCK_RAW, ntohs(ETH_P_IP));
    if (sock > 0)
    {
        return true;
    }
    return false;
}

void dataReceiver::changeSender(QHostAddress sender)
{
    this->sender = sender;
    expectedChunk = 0;
    lastFrameNo = 0;
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
        QByteArray array(1200, 0xff);
        ssize_t packetLength = recvfrom(sock, array.data(), 1200, 0, (struct sockaddr*)&src_addr, &addr_len);

        /*
            struct sockaddr_ll {
                unsigned short sll_family;   // Always AF_PACKET            => CHECK (= 17)
                unsigned short sll_protocol; // Physical layer protocol     => Don't know where to find constant or what it means, but we want 8
                int            sll_ifindex;  // Interface number            => Saved at first incoming packet and compared on all others
                unsigned short sll_hatype;   // ARP hardware type
                unsigned char  sll_pkttype;  // Packet type                 => Don't know where to find constant or what it means, but we want 2
                unsigned char  sll_halen;    // Length of address
                unsigned char  sll_addr[8];  // Physical layer address
            };
         */
        //qDebug() << "sll_family" << src_addr.sll_family << "sll_protocol" << src_addr.sll_protocol << "ifindex" << src_addr.sll_ifindex << "hatype" << src_addr.sll_hatype << "pkttype" << src_addr.sll_pkttype;

        // Early sanity checks
        if ((src_addr.sll_family != 17) || (src_addr.sll_protocol != 8) || (src_addr.sll_pkttype != 2))
        {
            continue;
        }

        // Incoming interface check
        if (ifindex == 0xFFFF)
        {
            // This is the first packet. Remember which interface we read it on
            ifindex = src_addr.sll_ifindex;
        }
        else if (ifindex != src_addr.sll_ifindex)
        {
            // Ignore packet which arrived on another interface than the first packet
            // In the VLAN use case, we receive every packet twice
            continue;
        }

        if (array.data()[23] != 0x11)
        {
            continue; // Not UDP
        }

        quint64* ethSourceAndProto = (quint64*)(array.data() + 6);
        if (*ethSourceAndProto != 0x0008016000780b00)
        {
            continue; // Source MAC or Proto (= IP) didn't match
        }

        quint32* source = (quint32*)(array.data() + 26);
        *source = ntohl(*source);
        if (*source != sender.toIPv4Address())
        {
            continue; // Source IP didn't match
        }

        quint32* dest = (quint32*)(array.data() + 30);
        if (*dest != 0x020202e2) continue; // Dest IP (= "226.2.2.2") didn't match

        quint32* ports = (quint32*)(array.data() + 34);
        if (*ports == 0x14081408)
        {
            // Video data (port = 2068)

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

            //qDebug() << "length:" << length << "frameNo:" << frameNo << "chunkNo:" << chunkNo << "lastChunk:" << lastChunk;

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
            // Audio data (port = 2066)

            // Payload starts after all headers (Ethernet + IPv4 + UDP); 16 byte of payload are skipped
            char* dataPtr = array.data() + 14 + 20 + 8 + 16;
            quint16 length = packetLength - (14 + 20 + 8 + 16);

            //qDebug() << "AUDIO CHUNK. Length:" << length << "MOD 8" << (length % 8);

            QByteArray data(dataPtr, length);
            emit newAudioChunk(data);
        }
    }
}

void dataReceiver::stopReceive()
{
    qDebug() << "DataReceiver stopping";
    run = false;
}
