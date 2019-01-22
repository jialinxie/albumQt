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

#define PICVIEWSIZE 77

AlbumWidget::AlbumWidget(QWidget *parent)
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
    , yPosLast(0),
  horizontalOffset(0),
  verticalOffset(0),
  rotationAngle(0),
  scaleFactor(1),
  currentStepScaleFactor(1)
{
    IMAGE_SIZE = QSize(PICVIEWSIZE, PICVIEWSIZE);
    ITEM_SIZE = QSize(PICVIEWSIZE, PICVIEWSIZE);

    setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // QListWidget基本设置
    pListShow = new picListShow(this);
    pListShow->setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - 30);
    pListShow->setIconSize(IMAGE_SIZE);
    pListShow->setResizeMode(QListView::Adjust);
    pListShow->setViewMode(QListView::IconMode);
    pListShow->setMovement(QListView::Static);
    pListShow->setSpacing(1);

    QScrollBar *scrollBar = pListShow->verticalScrollBar();
    pListShow->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pListShow->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    pListShow->setFocus();

    updateUI();
    connect(pListShow, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_itemClicked(QListWidgetItem*)));

    mShowWidget = new QLabel(this);

    menuButton = new QPushButton(this);
    menuButton->hide();
    backButton   = new QPushButton(this);
    backButton->hide();

    mShowWidget->setAutoFillBackground(true);
    mShowWidget->setGeometry(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);  //缓存两张图片
    mShowWidget->hide();

    setAttribute(Qt::WA_AcceptTouchEvents);
}

AlbumWidget::~AlbumWidget(){

}

void AlbumWidget::grabGestures(const QList<Qt::GestureType> &gestures)
{
    //! [enable gestures]
    foreach (Qt::GestureType gesture, gestures)
        grabGesture(gesture);
    //! [enable gestures]
}

void AlbumWidget::albumMenuSlot(int idx){
    printf("menu idx = %d", idx);
//    if(idx == 0)
//        picPropertiles();
//    else
//        deleteCurrentPic();
}

void AlbumWidget::focusInEvent(QFocusEvent *){
    int ret = updateUI();
}

void AlbumWidget::mousePressEvent(QMouseEvent *event)
{
    if (files.count() <= 0) {
        return;
    }

    mousePress = true;
    m_mouseSrcPos = event->pos();

//    xPosLast = mShowWidget->x();
//    yPosLast = mShowWidget->y();

//    if((m_mouseSrcPos.y() > SCREEN_HEIGHT-STATUS_BAR_HEIGHT) && (m_mouseSrcPos.x() > SCREEN_WIDTH-STATUS_BAR_HEIGHT)){
//        printf("clicked Menu!");
//        pSubFieldWidgetProperties.get()->show();
//    }

//    printf("Press isZoomMode, isFirstDouble, xPos = %d, yPos = %d, xPosLast = mShowWidget.x = %d, yPosLast = mShowWidget.y = %d\n",\
//                                       m_mouseSrcPos.x(),    m_mouseSrcPos.y(),   mShowWidget->x(),  mShowWidget->y());
}

void AlbumWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseDstPos = event->pos();
    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();
    int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();

    if(!isZoomMode){
        if (mouseMove) {
            if((curIndex == 0) && (xPos > 0))
                return;
            else
            if((curIndex == pListShow->count() - 1) && (xPos < 0))
                return;
            if(xPos > 100)   //100 is max tolerate distance for double press
                curIndex --;
            else
                if(xPos < -100)
                    curIndex ++;
            goToImage(curIndex);
        }
    }else{  //缩放模式下，当前缩放比例为1时，允许左右切换
        if(currentStepScaleFactor == 1){
            if((curIndex == 0) && (xPos > 0))
                return;
            else
            if((curIndex == pListShow->count() - 1) && (xPos < 0))
                return;
            if(xPos > 30)   //30 is max tolerate distance to load next img
                curIndex --;
            else
                if(xPos < -30)
                    curIndex ++;
            goToImage(curIndex);
        }
    }
}

void AlbumWidget::mouseMoveEvent(QMouseEvent *event)
{
    mouseMove = true;
    m_mouseDstPos = event->pos();

    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();
    int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();

//    if(isZoomMode){
//        int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();

//        double w = cenPixW * zoomScale;
//        double h = cenPixH * zoomScale;

//        if(xPos < 0){    //to left
//            if(xPos < (240 - w))    //单次向左滑动到达右边界时，禁止继续滑动
//                xPos = 240 - w;

//            if(mShowWidget->x() < (240 - w)){    //向左滑动到达图像右边界时，停止滑动
//                    xPosLast = 0;
//                    return;
//            }
//        }else
//            if(xPos > 0){//to right
//                if(mShowWidget->x() >= 0){ //向右滑动，图片到达左边界，禁止继续滑动
//                    xPosLast = 0;
//                    xPos = 0;
//                }
//            }

//        if(yPos < 0){   //to up
//            if(yPos < (320 - h))    //向上滑动到达右边界时，禁止继续滑动
//                yPos = 320 - h;

//            if(mShowWidget->y() < (320 - h)){    //向左滑动到达图像右边界时，停止滑动
//                    yPosLast = 0;
//                    return;
//            }
//        }else
//            if(yPos > 0){//to down
//                if(mShowWidget->y() >= 0){    //向下滑动，图片到达上边界，禁止继续滑动
//                    yPosLast = 0;
//                    yPos = 0;
//                }
//            }

//        mShowWidget->move(xPos + xPosLast, yPos + yPosLast);
//        mShowWidget->show();
//    }
}

void AlbumWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QSize screenSize(this->width(),this->height());
    QSize cenPicSize(currentImage.width(), currentImage.height());

    if(isSingleItemUI){                 //判断当前是否查看图像界面 查看图像界面/缩略图界面
        if(isFirstDouble){              //第一次双击，放大到填充屏幕
            qDebug() << "first double click!!";
            isZoomMode = true;
            isFirstDouble = false;
            scaleFactor = getScaleValue(cenPicSize, screenSize);
            horizontalOffset = (SCREEN_WIDTH / 2 - event->x()) / 2;
            verticalOffset   = (SCREEN_HEIGHT / 2 - event->y()) / 2;
            update();
            return;
        }else{   //第二次双击，返回slot_itemClicked
            qDebug() << "second double click!!!";
            isFirstDouble = true;
            isZoomMode = false;
            scaleFactor = 1;
            horizontalOffset = 0;
            verticalOffset = 0;
            update();
            return;
        }
    }
    else
        QWidget::mouseDoubleClickEvent(event);
}

void AlbumWidget::keyReleaseEvent(QKeyEvent *event)
{
    printf("Release! key = %d,currentRow = %d\n", event->key(), pListShow->currentRow());

    if(isSingleItemUI){
        switch (event->key()){
            case KEY_UP:
                curIndex -= 2;
                scaleFactor = 1;
            break;
            case KEY_DOWN:
                curIndex += 2;
                scaleFactor = 1;
            break;
            case KEY_LEFT:
                curIndex --;
                scaleFactor = 1;
            break;
            case KEY_RIGHT:
                curIndex ++;
                scaleFactor = 1;
            break;
            case KEY_RETURN: //back to list
                back2Album();
            break;
            case KEY_ENLARGE:{
                    QSize screenSize(this->width(),this->height());
                    QSize cenPicSize(currentImage.width(), currentImage.height());
                    scaleFactor = getScaleValue(cenPicSize, screenSize);
                }
            break;
            case KEY_NARROW:
                scaleFactor = 1;
            break;
            default:
                break;
        }

        if(curIndex < 0)
            curIndex = 0;
        if(curIndex > (files.count() - 1))
            curIndex = files.count() - 1;

        pListShow->setCurrentRow(curIndex);
        goToImage(curIndex);

    }else{  //on image list
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
}

bool AlbumWidget::gestureEvent(QGestureEvent *event){
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    else if (QGesture *pan = event->gesture(Qt::PanGesture))
        panTriggered(static_cast<QPanGesture *>(pan));
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;
}


void AlbumWidget::panTriggered(QPanGesture *gesture) {
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
    horizontalOffset += delta.x();
    verticalOffset += delta.y();
    update();
}

void AlbumWidget::pinchTriggered(QPinchGesture *gesture)
{
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

//! [swipe function]
void AlbumWidget::swipeTriggered(QSwipeGesture *gesture)
{
    if (gesture->state() == Qt::GestureFinished) {
        if (gesture->horizontalDirection() == QSwipeGesture::Left
            || gesture->verticalDirection() == QSwipeGesture::Up) {
//            qCDebug(lcExample) << "swipeTriggered(): swipe to previous";
            goPrevImage();
//            printf("swipe to previous\n");
        } else {
//            qCDebug(lcExample) << "swipeTriggered(): swipe to next";
            goNextImage();
//            printf("swipe to next\n");
        }
        update();
    }
}
//! [swipe function]

bool AlbumWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture){
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QWidget::event(event);
}

void AlbumWidget::paintEvent(QPaintEvent *event)
{
//    printf(__FUNCTION__);

    QPainter p(this);

    const qreal iw = currentImage.width();
    const qreal ih = currentImage.height();
    const qreal wh = height();
    const qreal ww = width();

    p.translate(ww/2, wh/2);
    p.translate(horizontalOffset, verticalOffset);
    p.rotate(rotationAngle);
    p.scale(currentStepScaleFactor * scaleFactor, currentStepScaleFactor * scaleFactor);
    p.translate(-iw/2, -ih/2);
    p.drawImage(0, 0, currentImage);
}

bool AlbumWidget::backEvent()
{
    if(isSingleItemUI){
        back2Album();
        return false;
    }
    else
        return true;
}

//gesturestart      // 当有两根或多根手指放到屏幕上的时候触发
//gesturechange     // 当有两根或多根手指在屏幕上，并且有手指移动的时候触发
//gestureend        // 当倒数第二根手指提起的时候触发，结束gesture

void AlbumWidget::setLabelMove(bool enable)
{
    label_move = enable;
}

void AlbumWidget::goNextImage()
{
    if (files.isEmpty())
        return;

    if (position < files.size()-1) {
        ++position;
        prevImage = currentImage;
        currentImage = nextImage;
        if (position+1 < files.size())
            nextImage = loadImage(ALBUM_PATH + files.at(position+1));
        else
            nextImage = QImage();
    }
    update();
}

void AlbumWidget::goPrevImage()
{
    if (files.isEmpty())
        return;

    if (position > 0) {
        --position;
        nextImage = currentImage;
        currentImage = prevImage;
        if (position > 0)
            prevImage = loadImage(ALBUM_PATH + files.at(position-1));
        else
            prevImage = QImage();
    }
    update();
}

void AlbumWidget::goToImage(int index){

    if (files.isEmpty())
        return;

    if (index < 0 || index >= files.size()) {
//        qDebug() << "goToImage: invalid index: " << index;
        return;
    }

    if (index == position+1) {
        goNextImage();
        return;
    }

    if (position > 0 && index == position-1) {
        goPrevImage();
        return;
    }

    position = index;

    if (index > 0)
        prevImage = loadImage(ALBUM_PATH + files.at(position-1));
    else
        prevImage = QImage();
    currentImage = loadImage(ALBUM_PATH + files.at(position));
    if (position+1 < files.size())
        nextImage = loadImage(ALBUM_PATH + files.at(position+1));
    else
        nextImage = QImage();
    update();
}

double AlbumWidget::getScaleValue(QSize img, QSize view)
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
/**
 * @brief AlbumWidget::updateBufferByKey
 * @param keyWay true:up false:down
 * always load 4x4 images to buffer
 */
void AlbumWidget::updateBufferByKey(bool keyWay)
{

}

int AlbumWidget::updateUI()
{
    files.clear();
    pListShow->clear();

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
    for (int i = 0; i<files.count(); ++i) {
        QPixmap pixmap(ALBUM_PATH + files.at(i));
        QListWidgetItem *listWidgetItem = new QListWidgetItem(QIcon(pixmap.scaled(IMAGE_SIZE)), NULL);  //delete name display
        listWidgetItem->setSizeHint(ITEM_SIZE);
        pListShow->insertItem(i, listWidgetItem);
    }
    if(pListShow->count() > 0){
        pListShow->setCurrentRow(curIndex);//默认光标在第一个
    }
    return 0;
}

QImage AlbumWidget::loadImage(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
//    qDebug() << "loading" << QDir::toNativeSeparators(fileName) << position << '/' << files.size();
    if (!reader.canRead()) {
//        qDebug() << QDir::toNativeSeparators(fileName) << ": can't load image";
        return QImage();
    }

    QImage image;
    if (!reader.read(&image)) {
//        qDebug() << QDir::toNativeSeparators(fileName) << ": corrupted image: " << reader.errorString();
        return QImage();
    }
    const QSize maximumSize(240, 320); // Reduce in case someone has large photo images.
    if (image.size().width() > maximumSize.width() || image.height() > maximumSize.height())
        image = image.scaled(maximumSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return image;
}

// 全屏等比例显示图像
void AlbumWidget::slot_itemClicked(QListWidgetItem * item){

    horizontalOffset        =   0;
    verticalOffset          =   0;
    scaleFactor             =   1;
    currentStepScaleFactor  =   1;

    isSingleItemUI = true;
	pListShow->hide();
    isFirstDouble = true;

    curIndex = pListShow->row(item);
    goToImage(curIndex);

//    //menu button
//    menuButton->setText("菜单");
//    menuButton->setGeometry(210, 300, 30, 20);
//    menuButton->raise();
//    connect(menuButton, SIGNAL(clicked()), this, SLOT(menuView()));

//    //back button
//    backButton->setText("返回");
//    backButton->setGeometry(0, 300, 30, 20);
//    backButton->raise();
//    connect(backButton, SIGNAL(clicked()), this, SLOT(back2Album()));
}

void AlbumWidget::menuView(void){
    qDebug() << "menuView";
}

void AlbumWidget::back2Album(void){
    //qDebug() << "back2Album";
    isSingleItemUI = false;
    isZoomMode = false;

    //disconnect(backButton, SIGNAL(clicked()), this, SLOT(back2Album()));
    mShowWidget->hide();
    //this->show();
    pListShow->show();
    if(pListShow->count() > 0){
        pListShow->setCurrentRow(curIndex);//默认光标在第一个
    }
//    printf("\nback2Album, count = %d\n", pListShow->count());
    this->show();
}

picListShow::picListShow(QWidget *parent):
isMove(false)
{
    this->setParent(parent);
}

picListShow::~picListShow()
{

}

bool picListShow::MoveStatus()
{
    return isMove;
}

bool picListShow::setMoveStatus()
{
    isMove = false;
}

void picListShow::mouseMoveEvent(QMouseEvent *event)
{
    isMove = true;
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
//    if(EndPoint == slidePoint)
//        itemClicked(this->selectedItems().at(0));   //    //sent itemClicked signal to ImageView
//    else
//        slidePoint = QPoint();  //resets the scroll drag


    QListWidget::mouseReleaseEvent(event);
}
