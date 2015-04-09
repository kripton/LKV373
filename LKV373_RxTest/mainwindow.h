#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QImage>

#include <QTime>
#include <QQueue>

#include "hdmireceiver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent * event);

private:
    Ui::MainWindow *ui;
    QGraphicsScene scene;
    QGraphicsPixmapItem* pixmapItem;

    QRect oldSize;

    HdmiReceiver* worker;

    QTime timer;
    QQueue<qreal> frameDiffs;

    qreal calculateFps();

public slots:
    void updateLabel(QByteArray frameData);
    void newControlPacket(QHostAddress sender, bool link, quint16 width, quint16 height, qreal fps);
    void selectedItemChanged(QString newText);
};

#endif // MAINWINDOW_H
