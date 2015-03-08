#ifndef LKV373APPSRC_H
#define LKV373APPSRC_H

#include <QObject>
#include <QByteArray>
#include <QTime>
#include <QQueue>

#include <QGst/Memory>
#include <QGst/Buffer>
#include <QGst/Utils/ApplicationSource>

class LKV373AppSrc : public QObject, public QGst::Utils::ApplicationSource
{
    Q_OBJECT

public:
    explicit LKV373AppSrc(QObject *parent = 0);
    ~LKV373AppSrc();

private:
    QTime timer;
    QQueue<qreal> frameDiffs;

    qreal calculateFps();

public slots:
    void newFrame(QByteArray frameData);
};

#endif // LKV373APPSRC_H
