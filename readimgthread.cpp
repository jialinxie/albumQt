#include "readimgthread.h"
#include <QDebug>
#include "albumwidget.h"

readImgThread::readImgThread()
{
    qDebug() << __func__;
//    pListShow
}

void readImgThread::run()
{
    qDebug() << __func__;
    AlbumWidget *img2Thread0 = AlbumWidget::getInstance(NULL);
    img2Thread0->updateUI();
    sendReadImgComplete();
}

void readImgThread::stop()
{
    qDebug() << __func__;
}
