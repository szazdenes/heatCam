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
    if((event->buttons() & Qt::LeftButton) != 0)
        emit signalLeftButtonPressed(mapToScene(event->pos()));
    else
        QGraphicsView::mousePressEvent(event);
}

void ImageGraphicsviewForm::mouseReleaseEvent(QMouseEvent *event)
{

}

void ImageGraphicsviewForm::mouseMoveEvent(QMouseEvent *event)
{
    emit signalMouseMoved(mapToScene(event->pos()));
}
