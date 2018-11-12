#include "mainwindow.h"
#include <QApplication>
#include <albumwidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AlbumWidget *imageListVew = new AlbumWidget();
    imageListVew->show();
    return a.exec();
}
