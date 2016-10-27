#include "imagegraphicsviewform.h"
#include "ui_imagegraphicsviewform.h"

ImageGraphicsviewForm::ImageGraphicsviewForm(QWidget *parent) :
   QGraphicsView(parent),
    ui(new Ui::ImageGraphicsviewForm)
{
    ui->setupUi(this);
    setMouseTracking(true);
}

ImageGraphicsviewForm::~ImageGraphicsviewForm()
{
    delete ui;
}

void ImageGraphicsviewForm::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit signalLeftButtonPressed(mapToScene(event->pos()));
    else
        QGraphicsView::mousePressEvent(event);
}

void ImageGraphicsviewForm::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        emit signalLeftButtonReleased(mapToScene(event->pos()));
    else
        QGraphicsView::mouseReleaseEvent(event);
}

void ImageGraphicsviewForm::mouseMoveEvent(QMouseEvent *event)
{
    emit signalMouseMoved(mapToScene(event->pos()));
}

void ImageGraphicsviewForm::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0)
        emit signalWheelUp();
    if(event->delta() < 0)
        emit signalWheelDown();
    else
        QGraphicsView::wheelEvent(event);
}
