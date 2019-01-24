#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>
#include "readimgthread.h"
class AlbumWidget;
class readImgThread;

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void grabGestures(const QList<Qt::GestureType> &gestures);

private:
    AlbumWidget *imageWidget;
    readImgThread thread0;
};

#endif // MAINWIDGET_H
