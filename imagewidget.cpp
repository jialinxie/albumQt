#include "imagewidget.h"
#include <QDir>
#include <QStringList>
#include <QListWidget>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QPushButton>
#include <stdbool.h>
#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPainter>
#include <QPalette>
#include <QGesture>
#include <QGestureEvent>
#include <QtMath>

#define picViewSize 78

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
    , mousePress(false)
    , mouseMove(false)
    , curIndex(0)
    , curPosX(-240)
    , isSingleItemUI(false)
    , isFirstDouble(true)
    , isZoomMode(false)
    , cenPixW(0)
    , cenPixH(0)
    , zoomScale(0.0)
    , xPosLast(0)
    , yPosLast(0)
{
    const QSize IMAGE_SIZE(picViewSize, picViewSize);
    const QSize ITEM_SIZE(picViewSize,  picViewSize);
    this->setGeometry(0, 0, 240, 320);

    m_strPath = "../Camera";

    QDir dir(m_strPath);
    if (!dir.exists()) {
        return;
    }
    // 设置过滤器
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList filters;
    filters << "*.png" << "*.jpg" <<"*.bmp";
    dir.setNameFilters(filters);
    m_imgList = dir.entryList();
    if (m_imgList.count() <= 0) {
        return;
    }

    // QListWidget基本设置
    pListShow = new picListShow(this);
    pListShow->setGeometry(0, 0, 240, 320);
    pListShow->setIconSize(IMAGE_SIZE);
    pListShow->setResizeMode(QListView::Adjust);
    pListShow->setViewMode(QListView::IconMode);
    pListShow->setMovement(QListView::Static);
    pListShow->setSpacing(0);

    QScrollBar *scrollBar = pListShow->verticalScrollBar();
    pListShow->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pListShow->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    // 创建单元项
    for (int i = 0; i<m_imgList.count(); ++i) {
        QPixmap pixmap(m_strPath + "/" + m_imgList.at(i));
        QListWidgetItem *listWidgetItem = new QListWidgetItem(QIcon(pixmap.scaled(IMAGE_SIZE)), NULL);  //delete name display
        listWidgetItem->setSizeHint(ITEM_SIZE);
        pListShow->insertItem(i, listWidgetItem);
    }

    // 信号与槽
    connect(pListShow, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_itemClicked(QListWidgetItem*)));
    mShowWidget = new QLabel(this);
    menuButton = new QPushButton(this);
    backButton   = new QPushButton(this);

    mShowWidget->setAutoFillBackground(true);
    mShowWidget->setGeometry(0, 0, 240 * 2, 320);  //缓存两张图片
    mShowWidget->hide();

    setAttribute(Qt::WA_AcceptTouchEvents);
}

ImageWidget::~ImageWidget(){

}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    mousePress = true;
    m_mouseSrcPos = event->pos();

    xPosLast = mShowWidget->x();
    yPosLast = mShowWidget->y();

//    printf("Press isZoomMode, isFirstDouble, xPos = %d, yPos = %d, xPosLast = mShowWidget.x = %d, yPosLast = mShowWidget.y = %d\n",\
//                                       m_mouseSrcPos.x(),    m_mouseSrcPos.y(),   mShowWidget->x(),  mShowWidget->y());
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseDstPos = event->pos();
    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();
    int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();

    if(!isZoomMode){
        if (mouseMove) {
            if((curIndex == 0) && (xPos > 0)){
                return;
            }else
            if((curIndex == pListShow->count() - 1) && (xPos < 0)){
                return;
            }

            if (xPos > 100) {                     //move right
                 curIndex --;
                 curPosX = -240;
            }else
            if(xPos < -100){
                 curIndex ++;
                 curPosX = 240;
            }

            mShowWidget->move(curPosX, 0);
            updateLoadImg(curIndex);
         }
    }else{
        if(isZoomMode){
            if(isFirstDouble){
                mShowWidget->resize(showPixmap.size());
                mShowWidget->setPixmap(showPixmap);

                if(curIndex == 0)
                    mShowWidget->move(0, 0);
                else
                    mShowWidget->move(-240, 0);

                mShowWidget->show();

//                printf("Release isZoomMode, isFirstDouble, xPos = %d, yPos = %d, mShowWidget.x = %d, .y = %d\n",\
//                                                   xPos,    yPos,   mShowWidget->x(),  mShowWidget->y());
            }
        }
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    mouseMove = true;
    m_mouseDstPos = event->pos();
    printf("yPos = %d \n", m_mouseDstPos.y());
    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();

    if(!isZoomMode){
        if((curIndex == 0) && (xPos > 0)){
            xPos = 0;
            curPosX = 0;
        }else
        if((curIndex == pListShow->count() - 1) && (xPos < 0)){
            xPos = 0;
            curPosX = 0;
        }

        if(curIndex == 0)
            mShowWidget->move(xPos, 0);
        else
            mShowWidget->move(xPos - 240, 0);
    }else{
        int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();

        double w = cenPixW * zoomScale;
        double h = cenPixH * zoomScale;

        if(xPos < 0){    //to left
            if(xPos < (240 - w))    //单次向左滑动到达右边界时，禁止继续滑动
                xPos = 240 - w;

            if(mShowWidget->x() < (240 - w)){    //向左滑动到达图像右边界时，停止滑动
                    xPosLast = 0;
                    return;
            }
        }else
            if(xPos > 0){//to right
                if(mShowWidget->x() >= 0){ //向右滑动，图片到达左边界，禁止继续滑动
                    xPosLast = 0;
                    xPos = 0;
                }
            }

        if(yPos < 0){   //to up
            if(yPos < (320 - h))    //向上滑动到达右边界时，禁止继续滑动
                yPos = 320 - h;

            if(mShowWidget->y() < (320 - h)){    //向左滑动到达图像右边界时，停止滑动
                    yPosLast = 0;
                    return;
            }
        }else
            if(yPos > 0){//to down
                if(mShowWidget->y() >= 0){    //向下滑动，图片到达上边界，禁止继续滑动
                    yPosLast = 0;
                    yPos = 0;
                }
            }

        mShowWidget->move(xPos + xPosLast, yPos + yPosLast);
        mShowWidget->show();

//        printf("Move isZoomMode, isFirstDouble, xPos = %d, yPos = %d, mShowWidget.x = %d, .y = %d, xPosLast = %d, yPosLast = %d\n",\
//                                           xPos,    yPos,   mShowWidget->x(),  mShowWidget->y(), xPosLast, yPosLast);
    }
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QSize screenSize(240, 320);
    QSize cenPicSize(cenPixW, cenPixH);

    if(isSingleItemUI){                 //判断当前是否查看图像界面 查看图像界面/缩略图界面
        if(isFirstDouble){              //第一次双击，放大到填充屏幕
            ////qDebug() << "first double click!!";
            isZoomMode = true;
            isFirstDouble = false;

            xPosLast = 0;
            yPosLast = 0;

            zoomScale = getScaleValue(cenPicSize, screenSize);

            double w = cenPixW * zoomScale;
            double h = cenPixH * zoomScale;

            mShowWidget->close();
            mShowWidget->setPixmap(cenPixmap.scaled(w, h, Qt::KeepAspectRatio));   //按比例缩放到(w, h)大小
            mShowWidget->resize(w, h);

            QPoint clickP = event->pos();
            //mShowWidget->move(-(clickP.x() * (scale - 1) / 2), - (clickP.y() * (scale - 1) / 2));    //需要以双击位置为中心点放大
            mShowWidget->move(0, 0);
            mShowWidget->show();

            return;
        }else{   //第二次双击，返回slot_itemClicked
            ////qDebug() << "second double click!!!";
            isFirstDouble = true;
            isZoomMode = false;
        }
    }
    else
        QWidget::mouseDoubleClickEvent(event);
}

void ImageWidget::gestureEvent(QGestureEvent *event){
    printf("gestureEvent\n");
}

void ImageWidget::touchEvent(QEvent *event)
{
    switch (event->type())
      {
        case QEvent::TouchBegin:
            //ui->FFLabel[1]->setText("Event began.");
            break;
        case QEvent::TouchEnd:
            //ui->FFLabel[1]->setText("Event ended.");
            break;
        case QEvent::TouchUpdate:
        {
            //ui->FFLabel[1]->setText("Event updated.");
            break;
        }
      }
}

//bool ImageWidget::event(QEvent *event)
//{
//    switch (event->type()) {
//    case QEvent::TouchBegin:
//    {
//        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
//        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
//        if(touchPoints.count() == 2){
//            printf("TouchBegin   OOOOOOOOOOOOOOO");
//        }
//        return true;
//    }
//    case QEvent::TouchUpdate:
//    {
//        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
//        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
//        if(touchPoints.count() == 2){
//            printf("TouchUpdate   KKKKKKKKKKKKKKK");
//        }
//        return true;
//    }
//    case QEvent::TouchEnd:
//    {
//        qDebug() <<"CProjectionPicture::event";
//        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
//        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
//        if(touchPoints.count() == 2){
//            printf("TouchEnd   KKKKKKKKKKKKKKK");
//        }
//        else if(touchPoints.count() == 1){
//            //m_bIsTwoPoint = false;
//        }
//        return true;
//    }
//    default:
//        break;
//    }

//    return QWidget::event(event);
//}


//gesturestart      // 当有两根或多根手指放到屏幕上的时候触发
//gesturechange     // 当有两根或多根手指在屏幕上，并且有手指移动的时候触发
//gestureend        // 当倒数第二根手指提起的时候触发，结束gesture

void ImageWidget::setLabelMove(bool enable)
{
    label_move = enable;
}

void ImageWidget::zoomOut()
{

}

void ImageWidget::zoomIn()
{

}

//void QListWidget::mouseMoveEvent(QMouseEvent *e){
//    qDebug() << "test";
//}

void ImageWidget::updateLoadImg(int index){
    int l = 0, r = 0;     //控制图片加载边界
    int xIndex;           //控制绘图的横坐标索引
    l = index;
    r = index;

    if((l < 0) || (r > (pListShow->count() - 1)))
        return;

    if(l > 0){                           //加载第一张以后的图片时，自动加载上一张
        xIndex = index - 1;
        l -= 1;
    }
    else
        xIndex = index;

    if(r < (pListShow->count() - 1)) //加载最后一张之前的图片时，自动加载上一张
        r += 1;

    QSize picSize(240,320); //设定屏幕比例参数

    QPixmap pixmap(QSize(this->width() * (r - l + 1), this->height()));
    QPainter painter(&pixmap);
    QImage image;

    pixmap.fill(Qt::black);
    //qDebug() << "l = " << l << " r = " << r << " xIndex = " << xIndex;

    //如果点击第一张图片，自动加载下一张，禁止右滑|如果点击最后一张，自动加载上一张，禁止左滑
    for(int i = l; i <= r; i++){
        //qDebug() << m_strPath + "/" + m_imgList.at(i);
        QImage image(m_strPath + "/" + m_imgList.at(i));
        QPixmap pixmap = QPixmap::fromImage(image).scaled(picSize, Qt::KeepAspectRatio);

        int h = pixmap.height();
        int w = pixmap.width();

        painter.drawPixmap((i - xIndex) * 240 + (240 - w) / 2, (320 - h) / 2, pixmap);

        if(index == i){
            cenPixW = w;
            cenPixH = h;
            cenPixmap = cenPixmap.fromImage(image); //KeepAspectRatio  KeepAspectRatioByExpanding , Qt::SmoothTransformation
        }
    }
    showPixmap = pixmap;

    mShowWidget->resize(showPixmap.size());
    mShowWidget->setPixmap(showPixmap);
    //加载2或3张图片后，总是从第0张开始显示，所以除了点击第一张图片，其它都要左移一个窗口宽度
    if(index == r)
        mShowWidget->move((l - r) * 240, 0);
    else
        mShowWidget->move((l - r + 1) * 240, 0);
}

double ImageWidget::getScaleValue(QSize img, QSize view)
{
    double w = view.width() * 1.0;
    double h = view.height()* 1.0;
    double wi = img.width();
    double hi = img.height();

    double x = w / wi;
    double y = h / hi;

    if((wi <= w) && (hi <= h)){
        if (x >= y)
            return x;
        else
            return y;
    }else
        if((wi > w) && (hi <= h)){
            //get: y > x
            return y;
        }else
            if((wi <= w) && (hi >=h)){
                //get: x > y
                return x;
            }else
                if((wi >= w) && (hi >= h)){
                    if(x > y)
                        return 1/x;
                    else
                        return 1/y;
                }
}

// 全屏等比例显示图像
void ImageWidget::slot_itemClicked(QListWidgetItem * item){
    //qDebug() << "slot_itemClicked, item index= " << pListShow->row(item) << "count = " << pListShow->count();
    isSingleItemUI = true;

    curIndex = pListShow->row(item);
    updateLoadImg(curIndex);

    //menu button
    menuButton->setText("菜单");
    menuButton->setGeometry(210, 300, 30, 20);
    connect(menuButton, SIGNAL(clicked()), this, SLOT(menuView()));

    //back button
    backButton->setText("返回");
    backButton->setGeometry(0, 300, 30, 20);

    //slot and singnal
    connect(backButton, SIGNAL(clicked()), this, SLOT(back2Album()));

    mShowWidget->show();
}

void ImageWidget::menuView(void){
    qDebug() << "menuView";
}

void ImageWidget::back2Album(void){
    //qDebug() << "back2Album";
    isSingleItemUI = false;
    isZoomMode = false;

    disconnect(backButton, SIGNAL(clicked()), this, SLOT(back2Album()));
    mShowWidget->hide();
    this->show();
}

picListShow::picListShow(QWidget *parent)
{
    qDebug() << "test!!!";
    this->setParent(parent);
}

picListShow::~picListShow()
{

}

void picListShow::mouseMoveEvent(QMouseEvent *event)
{
    if(!slidePoint.isNull()){
        bool direction = (slidePoint.y() < event->pos().y());
        int singleStep = (direction ? -4 : 4);
        this->verticalScrollBar()->setValue(verticalScrollBar()->value() + singleStep);
    }
}

void picListShow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        slidePoint = event->pos();
        yPos0 = slidePoint.x();
    }
    QListWidget::mousePressEvent(event);
}

void picListShow::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint EndPoint;
    EndPoint = event->pos();
    if(EndPoint == slidePoint)
        itemClicked(this->selectedItems().at(0));   //    //sent itemClicked signal to ImageView
    else
        slidePoint = QPoint();  //resets the scroll drag


    QListWidget::mouseReleaseEvent(event);
}
