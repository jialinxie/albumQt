#include "albumwidget.h"
#include <QLabel>
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
#include <qnamespace.h>
#include <QObject>
#include <assert.h>
#include <future>
#include <QPixmap>

#define PICVIEWSIZE 77


AlbumWidget::AlbumWidget(QWidget *parent)
    : QWidget(parent)
    , curIndex(0)
    , isMove(false)
{
    IMAGE_SIZE = QSize(PICVIEWSIZE, PICVIEWSIZE);
    ITEM_SIZE = QSize(PICVIEWSIZE, PICVIEWSIZE);

    backgroundLabel = new QLabel(this);
    backgroundLabel->setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    backgroundLabel->setStyleSheet("background-color:white");

    // QListWidget基本设置
    pListShow = new QListWidget(this);
    pListShow->setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - 30);
    pListShow->setIconSize(IMAGE_SIZE);
    pListShow->setResizeMode(QListView::Adjust);
    pListShow->setViewMode(QListView::IconMode);
    pListShow->setMovement(QListView::Static);
    pListShow->setSpacing(1);

    QScrollBar *scrollBar = pListShow->verticalScrollBar();
    pListShow->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pListShow->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    connect(pListShow, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_itemClicked(QListWidgetItem*)));
    menuButton = new QPushButton(this);
    menuButton->hide();
    backButton   = new QPushButton(this);
    backButton->hide();

    pSingleImgShow = new singleImgShow(this);
    pSingleImgShow->hide();

    setAttribute(Qt::WA_AcceptTouchEvents);
    this->setFocus();
}

AlbumWidget::~AlbumWidget(){

}

int AlbumWidget::getCurrentImageCount()
{
    files.clear();

    // 判断路径是否存在
    bool ret = dir.setCurrent(ALBUM_PATH);
    if (!dir.exists()) {
        return -1;
    }

    // 设置过滤器
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    filters << "*.png" << "*.jpg" <<"*.bmp";
    dir.setNameFilters(filters);
    files = dir.entryList();
    return files.count();
}

int AlbumWidget::getOldImageCount()
{
    return oldImageCount;
}

void AlbumWidget::setOldImageCount(int count)
{
    oldImageCount = count;
}

void AlbumWidget::albumMenuSlot(int idx){
    printf("menu idx = %d", idx);
//    if(idx == 0)
//        picPropertiles();
//    else
    //        deleteCurrentPic();
}

//void AlbumWidget::readImgCompleteSlot()
//{
//    qDebug() << "readImgComplete!!!!";
//}

void AlbumWidget::focusInEvent(QFocusEvent *){
    qDebug() << "AlbumWidget::" << __func__;
//        std::future<int> imgFuture = std::async(std::launch::async, &AlbumWidget::updateUI, this);
//        imgFuture.wait();
    updateUI();
}

void AlbumWidget::mousePressEvent(QMouseEvent *event)
{
    if (files.count() <= 0) {
        return;
    }

    m_mouseSrcPos = event->pos();

    if(event->button() == Qt::LeftButton){
        slidePoint = event->pos();
    }
}

void AlbumWidget::mouseReleaseEvent(QMouseEvent *event)
{
        QPoint EndPoint;
        EndPoint = event->pos();
}

void AlbumWidget::keyReleaseEvent(QKeyEvent *event)
{
    printf("AlbumWidget::Release! key = %d,currentRow = %d\n", event->key(), pListShow->currentRow());
    curIndex = pListShow->currentRow();
        pListShow->setCurrentRow(curIndex);

        switch (event->key()){
            case KEY_ENTER:
                slot_itemClicked(pListShow->currentItem());
            break;
            default:
                break;
        }
}

bool AlbumWidget::backEvent()
{
    pListShow->setCurrentRow(0);
}

void AlbumWidget::mouseMoveEvent(QMouseEvent *event)
{
        isMove = true;
        if(!slidePoint.isNull()){
            bool direction = (slidePoint.y() < event->pos().y());
            int singleStep = (direction ? -2 : 2);
            pListShow->verticalScrollBar()->setValue(pListShow->verticalScrollBar()->value() + singleStep);
//            if(fabs(slidePoint.x() - event->pos().x()) > 80){//80 滑动阀值
//                emit widgetSlide(!(slidePoint.x() > event->pos().x()));
//            }
        }
        QPoint p = event->pos() - slidePoint;

        m_moveLength = std::sqrt(std::pow(p.x(), 2) + std::pow(p.y(), 2));
}

bool AlbumWidget::moveStatus()
{
    return isMove;
}

void AlbumWidget::setMoveStatus(void)
{
    isMove = false;
}

double AlbumWidget::moveLength() const
{
    return m_moveLength;
}

int AlbumWidget::updateUI()
{
    files.clear();
    pListShow->clear();
    pSingleImgShow->pixmapList.clear();

    // 判断路径是否存在
    bool ret = dir.setCurrent(ALBUM_PATH);
    if (!dir.exists()) {
        return -1;
    }

    // 设置过滤器
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    filters << "*.png" << "*.jpg" <<"*.bmp";
    dir.setNameFilters(filters);
    files = dir.entryList();

    // 创建单元项
    for (int i = 0; i < files.count(); ++i) {
        qDebug() << "files.at(0).name  = " << files.at(i) << endl;
        img = new QImage(ALBUM_PATH + files.at(i));
        pixmap = new QPixmap(QPixmap::fromImage(img->scaled(ITEM_SIZE, Qt::KeepAspectRatio)));
        QListWidgetItem *listWidgetItem = new QListWidgetItem(QIcon(*pixmap), NULL);  //delete name display
        listWidgetItem->setSizeHint(ITEM_SIZE);
        pListShow->insertItem(i, listWidgetItem);
        pSingleImgShow->pixmapList.append(QPixmap::fromImage(img->scaled(QSize(240, 320), Qt::KeepAspectRatio)));
        delete pixmap;
        delete img;
    }
    if(pListShow->count() > 0){
        pListShow->setCurrentRow(curIndex);//默认光标在第一个
    }
    return 0;
}

// 全屏等比例显示图像
void AlbumWidget::slot_itemClicked(QListWidgetItem * item){
    qDebug() << __FUNCTION__ << endl;
    if(files.size() == 0){return;}
    if(curIndex < 0  || (curIndex > files.size() -1)){return;}

    if(this->moveStatus() && this->moveLength() > 20){    //disable to load single image when slide on album
        this->setMoveStatus();
        return;
    }

    curIndex = pListShow->row(item);

//    this->setStyleSheet("background-color:black");
    this->pListShow->hide();

    pSingleImgShow->goToImage(curIndex);

    pSingleImgShow->setFocus();
    pSingleImgShow->raise();
    pSingleImgShow->show();
}

void AlbumWidget::menuView(void){
    qDebug() << "menuView";
}

void AlbumWidget::back2Album(void){

}

//picListShow::picListShow(QWidget *parent):
//isMove(false)
//{
//    this->setParent(parent);
//    connect(&thread0, SIGNAL(sendReadImgComplete()), this, SLOT(readImgCompleteSlot()));
//}

////开机后第一次进相册才需要执行一次线程thread0, 后续检测相册文件数量若有变化,再次执行thread0
//void picListShow::focusInEvent(QFocusEvent *event)
//{
//    bool ret = thread0.isRunning();
//    if(!ret)
//        thread0.start();
//}

singleImgShow::singleImgShow(QWidget *parent):
    QWidget(parent),
    isZoomMode(false),
    horizontalOffset(0),
    verticalOffset(0),
    rotationAngle(0),
    scaleFactor(1),
    currentStepScaleFactor(1),
    isFirstDouble(true),
    position(0)
{
    setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    QList<Qt::GestureType> gestures;
    gestures << Qt::PanGesture;
    gestures << Qt::PinchGesture;
    gestures << Qt::SwipeGesture;
    this->grabGestures(gestures);

    setAttribute(Qt::WA_AcceptTouchEvents);
}

singleImgShow::~singleImgShow()
{

}

void singleImgShow::grabGestures(const QList<Qt::GestureType> &gestures)
{
    //! [enable gestures]
    foreach (Qt::GestureType gesture, gestures)
        grabGesture(gesture);
    //! [enable gestures]
}

void singleImgShow::focusInEvent(QFocusEvent *e)
{
    qDebug() << "singleImgShow::" << __FUNCTION__;
}

void singleImgShow::mousePressEvent(QMouseEvent *event)
{
    m_mouseSrcPos = event->pos();
}

void singleImgShow::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseDstPos = event->pos();
    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();
    int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();

    if(!isZoomMode && pixmapList.count() > 0){//滑动
        if (xPos > 60){  //60 是滑动阀值
            curIndex--;
        } else if (xPos < -60){
            curIndex++;
        }

        if(curIndex < 0){  curIndex = 0; }
        if(curIndex > (pixmapList.count() - 1)){ curIndex = pixmapList.count() - 1; }
        goToImage(curIndex);
    }else
    if(currentStepScaleFactor == 1){
        if((curIndex == 0) && (xPos > 0))
            return;
        else
        if((curIndex == pixmapList.count() - 1) && (xPos < 0))
            return;
        if(xPos > 30)   //30 is max tolerate distance to load next img
            curIndex --;
        else
            if(xPos < -30)
                curIndex ++;
        goToImage(curIndex);
    }
}

void singleImgShow::mouseDoubleClickEvent(QMouseEvent *event)
{
    QSize screenSize(this->width(),this->height());
    QSize cenPicSize(currentPixmap.width(), currentPixmap.height());
    if(isFirstDouble){              //第一次双击，放大到填充屏幕
        isZoomMode = true;
        isFirstDouble = false;
        scaleFactor = getScaleValue(cenPicSize, screenSize);
        if((int)scaleFactor == 1)
            scaleFactor = 2;
        currentStepScaleFactor = scaleFactor;
        scaleFactor = 1;
        horizontalOffset = (event->x() - SCREEN_WIDTH / 2) / 2;
        verticalOffset   = (event->y() - SCREEN_HEIGHT / 2) / 2;
        update();
        return;
    }else{   //第二次双击，返回slot_itemClicked
        isFirstDouble = true;
        isZoomMode = false;
        scaleFactor = 1;
        currentStepScaleFactor = 1;
        horizontalOffset = 0;
        verticalOffset = 0;
        update();
        return;
    }
}

void singleImgShow::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()){
        case KEY_UP:
            if(isZoomMode){
                moveImgHandler(0, 0, 5);
                return;
            }
            curIndex -= 2;
            scaleFactor = 1;
        break;
        case KEY_DOWN:
            if(isZoomMode){
                moveImgHandler(1, 0, -5);
                return;
            }
            curIndex += 2;
            scaleFactor = 1;
        break;
        case KEY_LEFT:
            if(isZoomMode){
                moveImgHandler(2, 5, 0);
                return;
            }
            curIndex --;
            scaleFactor = 1;
        break;
        case KEY_RIGHT:
            if(isZoomMode){
                moveImgHandler(3, -5, 0);
                return;
            }
            curIndex ++;
            scaleFactor = 1;
        break;
        case KEY_RETURN: {//back to list
            this->hide();
            AlbumWidget *pAlbum = AlbumWidget::getInstance(NULL);
            pAlbum->pListShow->raise();
            pAlbum->pListShow->setFocus();
            pAlbum->pListShow->show();
            return;
        }
        break;
        case KEY_ENLARGE:{
                QSize screenSize(this->width(),this->height());
                QSize cenPicSize(currentPixmap.width(), currentPixmap.height());
                scaleFactor = getScaleValue(cenPicSize, screenSize);
                if((int)scaleFactor == 1)
                    scaleFactor = 2;
            }
        break;
        case KEY_NARROW:
            scaleFactor = 1;
        break;
        default:
            break;
    }

    if(curIndex <= 0)
        curIndex = 0;
    else
        if(curIndex > (pixmapList.count() - 1))
            curIndex = pixmapList.count() - 1;

    goToImage(curIndex);
}

bool singleImgShow::gestureEvent(QGestureEvent *event)
{
    printf(__FUNCTION__);
    printf("\n");

    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    else if (QGesture *pan = event->gesture(Qt::PanGesture))
        panTriggered(static_cast<QPanGesture *>(pan));
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}

void singleImgShow::paintEvent(QPaintEvent *event)
{
    qDebug() << __FUNCTION__ << endl;
    QPainter pPainter(this);

    const qreal iw = currentPixmap.width();
    const qreal ih = currentPixmap.height();
    const qreal wh = height();
    const qreal ww = width();

    pPainter.translate(ww/2, wh/2);
    pPainter.translate(-horizontalOffset, -verticalOffset);
    pPainter.rotate(rotationAngle);
    pPainter.scale(currentStepScaleFactor * scaleFactor, currentStepScaleFactor * scaleFactor);
    pPainter.translate(-iw/2, -ih/2);
    pPainter.drawPixmap(0, 0, currentPixmap);
}

bool singleImgShow::backEvent()
{
    this->hide();
}

void singleImgShow::goNextImage()
{
    if (pixmapList.isEmpty())
        return;

    if (position < pixmapList.size()-1) {
        currentPixmap = pixmapList.at(++position);/*loadPixmap(ALBUM_PATH + pixmapList.at(++position));*/
    }
    update();
}

void singleImgShow::goPrevImage()
{
    if (pixmapList.isEmpty())
        return;

    if (position > 0) {
        currentPixmap = pixmapList.at(--position);/*loadPixmap(ALBUM_PATH + pixmapList.at(--position));*/
    }
    update();
}

void singleImgShow::goToImage(int index)
{
//    backgroundLabel->hide();

    if (index == position+1) {
        currentPixmap = pixmapList.at(++position);
    }else
    if (position > 0 && index == position-1) {
        currentPixmap = pixmapList.at(--position);
    }else{
        position = index;
        currentPixmap = pixmapList.at(position);
    }

    update();
}

double singleImgShow::getScaleValue(QSize img, QSize view)
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

void singleImgShow::menuView()
{

}

void singleImgShow::albumMenuSlot(int idx)
{

}

void singleImgShow::panTriggered(QPanGesture *gesture)
{
    printf(__FUNCTION__);
    printf("\n");

#ifndef QT_NO_CURSOR
    switch (gesture->state()) {
    case Qt::GestureStarted:
    case Qt::GestureUpdated:
        setCursor(Qt::SizeAllCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
    }
#endif
    QPointF delta = gesture->delta();
    horizontalOffset += (int)delta.x();
    verticalOffset += (int)delta.y();
    update();
}

void singleImgShow::pinchTriggered(QPinchGesture *gesture)
{
    printf(__FUNCTION__);
    printf("\n");
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
//    if (changeFlags & QPinchGesture::RotationAngleChanged) {
//        qreal rotationDelta = gesture->rotationAngle() - gesture->lastRotationAngle();
//        rotationAngle += rotationDelta;
//        qDebug() << "pinchTriggered(): rotate by" <<
//            rotationDelta << "->" << rotationAngle;
//    }
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        currentStepScaleFactor = gesture->totalScaleFactor();
//        qDebug() << "pinchTriggered(): zoom by" <<
//            gesture->scaleFactor() << "->" << currentStepScaleFactor;
//        printf("pinchTriggered  zoom by %0.2f -> %0.2f \n", gesture->scaleFactor(),currentStepScaleFactor);
    }
    if (gesture->state() == Qt::GestureFinished) {
        if(currentStepScaleFactor < 1)
            currentStepScaleFactor = 1;
        scaleFactor *= currentStepScaleFactor;
        currentStepScaleFactor = 1;
    }
    update();
}

void singleImgShow::swipeTriggered(QSwipeGesture *gesture)
{
    if (gesture->state() == Qt::GestureFinished) {
        if (gesture->horizontalDirection() == QSwipeGesture::Left
            || gesture->verticalDirection() == QSwipeGesture::Up) {
//            goPrevImage();
//            printf("swipe to previous\n");
        } else {
//            goNextImage();
//            printf("swipe to next\n");
        }
        update();
    }
}

void singleImgShow::moveImgHandler(int direction, int horiz, int verti)
{
    if(direction < 0 || direction > 3)
        return;

    if((horizontalOffset - horiz > SCREEN_WIDTH / 4) || (horizontalOffset - horiz < -SCREEN_WIDTH / 4))
        return;

    if(verticalOffset - verti > SCREEN_HEIGHT / 4 || verticalOffset - verti < -SCREEN_HEIGHT / 4)
        return;

    switch(direction){
        case 0: verticalOffset -= verti;
            break;
        case 1: verticalOffset -= verti;
            break;
        case 2: horizontalOffset -= horiz;
            break;
        case 3: horizontalOffset -= horiz;
            break;
        default:
            break;
    }

    if(horizontalOffset < -SCREEN_WIDTH / 4){
        horizontalOffset = -SCREEN_WIDTH / 4;
    }

    if(horizontalOffset > SCREEN_WIDTH / 4){
        horizontalOffset = SCREEN_WIDTH / 4;
    }

    if(verticalOffset < -SCREEN_HEIGHT / 4){
        horizontalOffset = -SCREEN_HEIGHT / 4;
    }

    if(verticalOffset > SCREEN_HEIGHT / 4){
        horizontalOffset = SCREEN_HEIGHT / 4;
    }

    update();
}
