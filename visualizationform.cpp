#include "visualizationform.h"
#include "ui_visualizationform.h"

VisualizationForm::VisualizationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisualizationForm)
{
    ui->setupUi(this);
    connect(this, &VisualizationForm::signalXlsToCsv, this, &VisualizationForm::slotXlsToCsv);

    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    zoom = 1;
    tmin = 0;
    tmax = 60;
}

VisualizationForm::~VisualizationForm()
{
    delete ui;
}

void VisualizationForm::on_loadPushButton_clicked()
{
    QFile heatFile(QFileDialog::getOpenFileName(this, "Open heat data", "../../", "*.xls"));
    if(!heatFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("baj");
        return;
    }

    QFile heatFilecsv;
    if(heatFile.fileName().endsWith(".xls")){
        heatFilenamecsv = QString(heatFile.fileName()).replace(".xls", ".csv");
        if(!QFile(heatFilenamecsv).exists())
            emit signalXlsToCsv(heatFile);
        if(QFile(heatFilenamecsv).exists())
            heatFilecsv.setFileName(heatFilenamecsv);
        else
            return;
    }

    heatFile.close();

    if(!heatFilecsv.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("baj");
        return;
    }

    QTextStream read(&heatFilecsv);
    QString line;
    int count = 1;

    while(!read.atEnd()){
        line = read.readLine();

        if(line.startsWith("0") || line.startsWith("1") || line.startsWith("2") || line.startsWith("3") || line.startsWith("4") || line.startsWith("5") || line.startsWith("6") || line.startsWith("7") || line.startsWith("8") || line.startsWith("9")){
            QStringList lineData = QString(line).split(",");
            heatMatrixMap[count] = lineData;
            count++;
        }
    }

    drawHeatMap(heatMatrixMap);

}

void VisualizationForm::slotXlsToCsv(QFile &file)
{
    QProcess proc;
    QString filename = file.fileName();
    QString filenamecsv(QString(filename).replace(".xls", ".csv"));
    QString cmd("ssconvert " + filename + " " + filenamecsv);
    proc.execute(cmd);
    proc.waitForFinished();
    proc.close();
}

void VisualizationForm::drawHeatMap(QMap<int, QStringList> &heatMap)
{

    int width = heatMap[heatMap.keys().first()].size();
    int height = heatMap.keys().size();
    image = QImage(width+60, height, QImage::Format_ARGB32_Premultiplied);
    for(int w = 0; w < width; w++){
        for(int h = 0; h < height; h++){
            QColor pixColor;
            double temp = QString(heatMap[h+1].at(w)).toDouble();
            if(temp == tmin + (tmax-tmin)/2.0)
                pixColor = QColor(Qt::white);
            if(temp < tmin + (tmax-tmin)/2.0){
                if(temp < tmin || temp > tmax)
                    pixColor = QColor(Qt::green);
                else
                    pixColor.setRgbF((temp-tmin)/((tmax-tmin)/2.0), (temp-tmin)/((tmax-tmin)/2.0), 1);
            }
            if(temp > tmin + (tmax-tmin)/2.0){
                if(temp < tmin || temp > tmax)
                    pixColor = QColor(Qt::green);
                else
                    pixColor.setRgbF(1, 2 - (temp-tmin)/((tmax-tmin)/2.0), 2 - (temp-tmin)/((tmax-tmin)/2.0));
            }
            image.setPixelColor(w, h, pixColor);
        }
    }

    QPainter painter(&image);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidthF(0.5);
    painter.setPen(pen);

    for(int w = width+2; w <= width+28; w++){
        for(int h = 0; h < height; h++){
            QColor pixColor;
            double temp = tmax - h*(tmax-tmin)/(height-1);
            if(temp == tmin + (tmax-tmin)/2.0)
                pixColor = QColor(Qt::white);
            if(temp < tmin + (tmax-tmin)/2.0){
                if(temp < tmin || temp > tmax)
                    pixColor = QColor(Qt::green);
                else
                    pixColor.setRgbF((temp-tmin)/((tmax-tmin)/2.0), (temp-tmin)/((tmax-tmin)/2.0), 1);
            }
            if(temp > tmin + (tmax-tmin)/2.0){
                if(temp < tmin || temp > tmax)
                    pixColor = QColor(Qt::green);
                else
                    pixColor.setRgbF(1, 2 - (temp-tmin)/((tmax-tmin)/2.0), 2 - (temp-tmin)/((tmax-tmin)/2.0));
            }
            image.setPixelColor(w, h, pixColor);
            if((int)temp % 10 == 0){
                painter.setFont(QFont("SansSerif", 10, QFont::Normal));
                painter.drawText(QRect(width+31, h-1, 29, 10), Qt::AlignCenter | Qt::AlignVCenter, QString::number((int)temp));
            }
        }
    }


    scene.clear();
    scene.addPixmap(QPixmap::fromImage(image));


}

void VisualizationForm::on_fiPushButton_clicked()
{
    ui->graphicsView->scale(1.0/zoom, 1.0/zoom);

    QRectF imageRect = image.rect();
    QRectF rect = ui->graphicsView->viewport()->rect();
    double fitSize = qMin<double>(rect.width() / imageRect.width(), rect.height() / imageRect.height());

    ui->graphicsView->scale(fitSize, fitSize);
    zoom = fitSize;
}

void VisualizationForm::on_origialPushButton_clicked()
{
    ui->graphicsView->scale(1.0/zoom, 1.0/zoom);
    zoom = 1;
}

void VisualizationForm::on_savePushButton_clicked()
{
    QString saveName = heatFilenamecsv.replace(".csv", ".png");
    image.save(saveName);
}

void VisualizationForm::on_tempPushButton_clicked()
{
    TemperatureDialog temp;
    connect(&temp, &TemperatureDialog::signalSendMinMax, this, &VisualizationForm::slotTempMinMax);
    temp.exec();
}

void VisualizationForm::slotTempMinMax(double min, double max)
{
    tmin = min;
    tmax = max;
    drawHeatMap(heatMatrixMap);
}
