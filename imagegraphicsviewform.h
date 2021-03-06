#ifndef IMAGEGRAPHICSVIEWFORM_H
#define IMAGEGRAPHICSVIEWFORM_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>

namespace Ui {
class ImageGraphicsviewForm;
}

class ImageGraphicsviewForm : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageGraphicsviewForm(QWidget *parent = 0);
    ~ImageGraphicsviewForm();

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

signals:
    void signalLeftButtonPressed(QPointF pos);
    void signalLeftButtonReleased(QPointF pos);
    void signalMouseMoved(QPointF pos);
    void signalWheelUp();
    void signalWheelDown();

private:
    Ui::ImageGraphicsviewForm *ui;
};

#endif // IMAGEGRAPHICSVIEWFORM_H
