#ifndef HEATMAPDIALOG_H
#define HEATMAPDIALOG_H

#include <QDialog>
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTextStream>
#include <QFile>
#include <QDebug>

namespace Ui {
class HeatMapDialog;
}

class HeatMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HeatMapDialog(QWidget *parent = nullptr);
    ~HeatMapDialog();

signals:

public slots:
    void slotShowHeatImage(QString heatImageName);

private slots:
    void on_fitPushButton_clicked();

    void on_applyPixelcutPushButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::HeatMapDialog *ui;

    void updatePreview();
    void createHeatmapCsv(QImage &image);

    QImage previewImage;
    QString previewImageName;
    double zoom;
};

#endif // HEATMAPDIALOG_H
