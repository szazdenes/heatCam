#include "heatmapdialog.h"
#include "ui_heatmapdialog.h"

HeatMapDialog::HeatMapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HeatMapDialog)
{
    ui->setupUi(this);
    ui->maxTempSpinBox->setValue(100);
    zoom = 1;
}

HeatMapDialog::~HeatMapDialog()
{
    delete ui;
}

void HeatMapDialog::slotShowHeatImage(QString heatImageName)
{
    previewImage = QImage(heatImageName);
    previewImageName = heatImageName;
    updatePreview();

}

void HeatMapDialog::updatePreview()
{
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(previewImage));
    scene->addItem(item);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
}

void HeatMapDialog::createHeatmapCsv(QImage &image)
{
    QFile exportFile(previewImageName.remove(".jpg"));
    exportFile.setFileName(exportFile.fileName() + "_heatmap.csv");
    if(!exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug("baj");
        return;
    }
    double min = ui->minTempSpinBox->value();
    double max = ui->maxTempSpinBox->value();

    QTextStream out(&exportFile);
    for(int k = 1; k < 10; k++) out << QString("Empty line ") + QString::number(k) + "\n";
    for(int j = 0; j < image.height(); j++){
        for(int i = 0; i < image.width(); i++){
            double greyness = image.pixelColor(i, j).blackF();
            double temp = (max - min) * greyness + min;
//            qDebug() << QString::number(greyness) + "," + QString::number(temp) + "\n";
            if(i == image.width() - 1) out <<  QString::number(temp) + "\n";
            else out << QString::number(temp) + ",";
        }
    }
    exportFile.close();
}

void HeatMapDialog::on_fitPushButton_clicked()
{
    ui->graphicsView->scale(1.0/zoom, 1.0/zoom);

    QRectF imageRect = previewImage.rect();
    QRectF rect = ui->graphicsView->viewport()->rect();
    double fitSize = qMin<double>(rect.width() / imageRect.width(), rect.height() / imageRect.height());

    ui->graphicsView->scale(fitSize, fitSize);
    zoom = fitSize;

}


void HeatMapDialog::on_applyPixelcutPushButton_clicked()
{
    int width = previewImage.width();
    int height = previewImage.height();
    int cutpixel = ui->cutPixelsSpinBox->value();

    QImage newImage = QImage(width - cutpixel, height, QImage::Format_ARGB32_Premultiplied);

    for(int i = 0; i < width - cutpixel; i++){
        for(int j = 0; j < height; j++){
            newImage.setPixel(i, j, previewImage.pixel(i,j));
        }
    }

    previewImage = newImage;
    updatePreview();
}


void HeatMapDialog::on_buttonBox_accepted()
{
    createHeatmapCsv(previewImage);
}

