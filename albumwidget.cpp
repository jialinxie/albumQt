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

    connect(pListShow, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slot_itemClicked(QListWidgetItem*)));
    menuButton = new QPushButton(this);
    menuButton->hide();
    backButton   = new QPushButton(this);
    backButton->hide();

    QList<Qt::GestureType> gestures;
    gestures << Qt::PanGesture;
    gestures << Qt::PinchGesture;
    gestures << Qt::SwipeGesture;
    this->grabGestures(gestures);

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

void AlbumWidget::show()
{
    if(!isReturnFromSingleUI){
        if(files.count() > 0){
            pListShow->setCurrentRow(0);
        }
    }else{
        isReturnFromSingleUI = false;
    }

    QWidget::show();
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
    qDebug() << __func__;
//    int ret = updateUI();
	pListShow->setFocus();  //Response navigation key
}

void AlbumWidget::mousePressEvent(QMouseEvent *event)
{
    if (files.count() <= 0) {
        return;
    }

    mousePress = true;
    m_mouseSrcPos = event->pos();
}

void AlbumWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseDstPos = event->pos();
    int xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();
    int yPos = m_mouseDstPos.y() - m_mouseSrcPos.y();

    if(!isZoomMode){
        if(isSingleItemUI && files.count() > 0){//滑动
            if (xPos > 60){  //60 是滑动阀值
                curIndex--;
            } else if (xPos < -60){
                curIndex++;
            }

            if(curIndex < 0){  curIndex = 0; }
            if(curIndex > (files.count() - 1)){ curIndex = files.count() - 1; }

            pListShow->setCurrentRow(curIndex);
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

void AlbumWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QSize screenSize(this->width(),this->height());
    QSize cenPicSize(currentPixmap.width(), currentPixmap.height());
    if(isSingleItemUI){                 //判断当前是否查看图像界面 查看图像界面/缩略图界面
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
    else
        QWidget::mouseDoubleClickEvent(event);
}

void AlbumWidget::keyReleaseEvent(QKeyEvent *event)
{
    printf("Release! key = %d,currentRow = %d\n", event->key(), pListShow->currentRow());

    if(isSingleItemUI){
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
            case KEY_RETURN: //back to list
                back2Album();
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


void AlbumWidget::panTriggered(QPanGesture *gesture) {
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

void AlbumWidget::pinchTriggered(QPinchGesture *gesture)
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

//! [swipe function]
void AlbumWidget::swipeTriggered(QSwipeGesture *gesture)
{
    if (gesture->state() == Qt::GestureFinished) {
        if (gesture->horizontalDirection() == QSwipeGesture::Left
            || gesture->verticalDirection() == QSwipeGesture::Up) {
            goPrevImage();
//            printf("swipe to previous\n");
        } else {
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
    printf(__FUNCTION__);
    printf("\n");
    QPainter p(this);

    const qreal iw = currentPixmap.width();
    const qreal ih = currentPixmap.height();
    const qreal wh = height();
    const qreal ww = width();

    p.translate(ww/2, wh/2);
    p.translate(-horizontalOffset, -verticalOffset);
    p.rotate(rotationAngle);
    p.scale(currentStepScaleFactor * scaleFactor, currentStepScaleFactor * scaleFactor);
    p.translate(-iw/2, -ih/2);
    p.drawPixmap(0, 0, currentPixmap);
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

void AlbumWidget::setLabelMove(bool enable)
{
    label_move = enable;
}

void AlbumWidget::goNextImage()
{
    if (files.isEmpty())
        return;

    if (position < files.size()-1) {
        currentPixmap = pixmapList.at(++position);/*loadPixmap(ALBUM_PATH + files.at(++position));*/
    }
    update();
}

void AlbumWidget::goPrevImage()
{
    if (files.isEmpty())
        return;

    if (position > 0) {
        currentPixmap = pixmapList.at(--position);/*loadPixmap(ALBUM_PATH + files.at(--position));*/
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
/**
 * @brief AlbumWidget::moveImgHandler
 * @param direction 0 = up, 1 = down, 2 = left, 3 = right
 * @param horiz
 * @param verti
 */
void AlbumWidget::moveImgHandler(int direction, int horiz, int verti)
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

int AlbumWidget::updateUI()
{
    files.clear();
    pListShow->clear();
    pixmapList.clear();

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
        QPixmap pixmap(ALBUM_PATH + files.at(i));
        QListWidgetItem *listWidgetItem = new QListWidgetItem(QIcon(pixmap.scaled(IMAGE_SIZE)), NULL);  //delete name display
        listWidgetItem->setSizeHint(ITEM_SIZE);
        pListShow->insertItem(i, listWidgetItem);
        pixmapList.append(pixmap.scaled(QSize(240, 320), Qt::KeepAspectRatio));
    }
    if(pListShow->count() > 0){
        pListShow->setCurrentRow(curIndex);//默认光标在第一个
    }
    return 0;
}

QImage AlbumWidget::loadImage(const QString &fileName)
{
    const QSize maximumSize(240, 320); // Reduce in case someone has large photo images.

    QImage image;
    image.load(fileName);

    QPixmap pixmapToShow = QPixmap::fromImage(image, Qt::AutoColor);

//    const QSize maximumSize(240, 320); // Reduce in case someone has large photo images.
//    if (image.size().width() > maximumSize.width() || image.height() > maximumSize.height())
//        image = image.scaled(maximumSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return image;
}

QPixmap AlbumWidget::loadPixmap(const QString &fileName)
{
    QIcon currentIcon = pListShow->item(curIndex)->icon();
    QPixmap pixmapToShow = currentIcon.pixmap(currentIcon.actualSize(QSize(1944, 2592)));
    pixmapToShow.scaled(240, 320, Qt::KeepAspectRatio, Qt::FastTransformation);
    return pixmapToShow;
}

// 全屏等比例显示图像
void AlbumWidget::slot_itemClicked(QListWidgetItem * item){
    qDebug() << "curIndex = " << curIndex;
    if(files.size() == 0){return;}
    if(pListShow->MoveStatus() && pListShow->moveLength() > 20){    //disable to load single image when slide on album
        pListShow->setMoveStatus();
        return;
    }

    horizontalOffset        =   0;
    verticalOffset          =   0;
    scaleFactor             =   1;
    currentStepScaleFactor  =   1;

    isSingleItemUI = true;
	pListShow->hide();
    isFirstDouble = true;

    curIndex = pListShow->row(item);

    if(curIndex < 0  || (curIndex > files.size() -1)){return;}

    printf("AlbumWidget::slot_itemClicked curIndex = %d \n",curIndex);

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
//    printf("AlbumWidget::back2Album  &&&&&&&&&&&&&&&  \n");
    isReturnFromSingleUI = true;
    isSingleItemUI = false;
    isZoomMode = false;
    pListShow->show();
    pListShow->setCurrentRow(curIndex);
    this->show();
}

picListShow::picListShow(QWidget *parent):
isMove(false)
{
    this->setParent(parent);
    connect(&thread0, SIGNAL(sendReadImgComplete()), this, SLOT(readImgCompleteSlot()));
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
        int singleStep = (direction ? -2 : 2);
        this->verticalScrollBar()->setValue(verticalScrollBar()->value() + singleStep);
        if(fabs(slidePoint.x() - event->pos().x()) > 80){//80 滑动阀值
            emit widgetSlide(!(slidePoint.x() > event->pos().x()));
        }
    }
    QPoint p = event->pos() - slidePoint;

    m_moveLength = std::sqrt(std::pow(p.x(), 2) + std::pow(p.y(), 2));
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

   QListWidget::mouseReleaseEvent(event);
}
//开机后第一次进相册才需要执行一次线程thread0, 后续检测相册文件数量若有变化,再次执行thread0
void picListShow::focusInEvent(QFocusEvent *event)
{
    bool ret = thread0.isRunning();
    if(!ret)
        thread0.start();
}

void picListShow::focusOutEvent(QFocusEvent *event)
{
    bool ret = thread0.isRunning();
    bool ret1=  thread0.isFinished();
//    if(ret1)
//        connect(&thread0, SIGNAL(finished()), &thread0, SLOT(deleteLater()));
}

double picListShow::moveLength() const
{
    return m_moveLength;
}

void picListShow::readImgCompleteSlot()
{

}

