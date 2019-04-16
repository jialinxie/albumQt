#include "readimgthread.h"
#include <QDebug>
#include "albumwidget.h"
#include <sys/time.h>

readImgThread::readImgThread()
{
    qDebug() << __func__;
}

void readImgThread::run()
{
    qDebug() << __func__;
    timeval time0, time1;
    gettimeofday(&time0, NULL);

    AlbumWidget *imageWidget0 = AlbumWidget::getInstance(NULL);

    if(imageWidget0->getOldImageCount() != imageWidget0->getCurrentImageCount()){
        imageWidget0->setOldImageCount(imageWidget0->getCurrentImageCount());
        imageWidget0->updateUI();
    }


    gettimeofday(&time1, NULL);

    qDebug() << "run time = " + QString::number((time1.tv_sec - time0.tv_sec) * 1000000 + (time1.tv_sec - time0.tv_sec)) + "us";
    sendReadImgComplete();
}

void readImgThread::stop()
{
    qDebug() << __func__;
}
