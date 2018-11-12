#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "albumwidget.h"

MainWindow::MainWindow(QWidget *parent)
{
    AlbumWidget *imageListVew = new AlbumWidget(this);
    imageListVew->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
