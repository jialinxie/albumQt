#include "mainwidget.h"
#include "albumwidget.h"

MainWidget::MainWidget(QWidget *parent)
//, imageWidget(new AlbumWidget(this))
{
    setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    imageWidget = AlbumWidget::getInstance(this);
    imageWidget->show();
}

MainWidget::~MainWidget()
{

}
