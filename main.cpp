#include "mainwindow.h"
#include <QApplication>
#include <imagewidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageWidget *imageListVew = new ImageWidget();
    imageListVew->show();

    return a.exec();
}
