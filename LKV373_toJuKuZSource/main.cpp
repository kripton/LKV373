#include "worker.h"
#include <QtCore>
#include <QGst/Init>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QGst::init(&argc, &argv);

    Worker worker;

    return a.exec();
}
