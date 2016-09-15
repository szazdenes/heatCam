#ifndef IMAGEGRAPHICSVIEWFORM_H
#define IMAGEGRAPHICSVIEWFORM_H

#include <QWidget>

namespace Ui {
class ImageGraphicsviewForm;
}

class ImageGraphicsviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit ImageGraphicsviewForm(QWidget *parent = 0);
    ~ImageGraphicsviewForm();

private:
    Ui::ImageGraphicsviewForm *ui;
};

#endif // IMAGEGRAPHICSVIEWFORM_H
