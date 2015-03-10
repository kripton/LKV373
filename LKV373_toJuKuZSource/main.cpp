#include "worker.h"
#include <QtCore>
#include <QApplication>
#include <QGst/Init>

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        qDebug() << "Usage: LKV373_toJuKuZSource <SENDERs_IP> <LOCALBIND_IP> <FORCEDFRAMERATE (or 0)>";
    }

    QApplication a(argc, argv);
    QGst::init(&argc, &argv);

    QString sender_address = QString::fromUtf8(argv[1]);
    Worker* worker = new Worker(sender_address, 0);
    worker->outgoingIP = QString::fromUtf8(argv[2]);
    worker->forcedFramerate = QString::fromUtf8(argv[3]).toInt();

    return a.exec();
}
