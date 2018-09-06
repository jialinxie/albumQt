#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "imagewidget.h"

MainWindow::MainWindow(QWidget *parent)
{
    ImageWidget *imageListVew = new ImageWidget(this);
    imageListVew->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
