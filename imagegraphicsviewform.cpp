#include "imagegraphicsviewform.h"
#include "ui_imagegraphicsviewform.h"

ImageGraphicsviewForm::ImageGraphicsviewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageGraphicsviewForm)
{
    ui->setupUi(this);
}

ImageGraphicsviewForm::~ImageGraphicsviewForm()
{
    delete ui;
}
