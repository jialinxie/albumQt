#include "mainwidget.h"
#include "albumwidget.h"

MainWidget::MainWidget(QWidget *parent)
//, imageWidget(new AlbumWidget(this))
{
    setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    imageWidget = new AlbumWidget(this);
    imageWidget->show();
}

MainWidget::~MainWidget()
{

}

void MainWidget::grabGestures(const QList<Qt::GestureType> &gestures)
{
    imageWidget->grabGestures(gestures);
}
