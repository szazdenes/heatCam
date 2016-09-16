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
    paletteNum = 1;
}

VisualizationForm::~VisualizationForm()
{
    delete image;
    delete ui;
}

void VisualizationForm::on_loadPushButton_clicked()
{
    heatMatrixMap.clear();
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
    QString dir = QString(filename).remove(filename.split("/").last());
    QString cmd("libreoffice --headless --convert-to csv " + filename + " --outdir " + dir);
    proc.execute(cmd);
    proc.waitForFinished();
    proc.close();
}

void VisualizationForm::drawHeatMap(QMap<int, QStringList> &heatMap)
{
    if(!heatMap.isEmpty()){
        delete image;
        int width = heatMap[heatMap.keys().first()].size();
        int height = heatMap.keys().size();
        image = new QImage(width+60, height+5, QImage::Format_ARGB32_Premultiplied);
        image->fill(Qt::white);
        for(int w = 0; w < width; w++){
            for(int h = 0; h < height; h++){
                QColor pixColor;
                double temp = QString(heatMap[h+1].at(w)).toDouble();
                pixColor = getPixelColor(paletteNum, temp, tmin, tmax);
                image->setPixelColor(w, h, pixColor);
            }
        }

        QPainter painter(image);



        for(int w = width+2; w <= width+28; w++){
            for(int h = 0; h < height; h++){
                QColor pixColor;
                double temp = tmax - h*(tmax-tmin)/(height-1);
                pixColor = getPixelColor(paletteNum, temp, tmin, tmax);
                image->setPixelColor(w, h, pixColor);
            }
        }

        painter.drawText(QRect(width+31, height-5*(double)height/5.0, 29, 15), Qt::AlignCenter | Qt::AlignVCenter, "°C");
        painter.drawText(QRect(width+31, height-10-4*(double)height/5.0, 29, 15), Qt::AlignCenter | Qt::AlignVCenter, QString::number(qRound(tmax - (height-4*(double)height/5.0)*(tmax-tmin)/(height))));
        painter.drawText(QRect(width+31, height-10-3*(double)height/5.0, 29, 15), Qt::AlignCenter | Qt::AlignVCenter, QString::number(qRound(tmax - (height-3*(double)height/5.0)*(tmax-tmin)/(height))));
        painter.drawText(QRect(width+31, height-10-2*(double)height/5.0, 29, 15), Qt::AlignCenter | Qt::AlignVCenter, QString::number(qRound(tmax - (height-2*(double)height/5.0)*(tmax-tmin)/(height))));
        painter.drawText(QRect(width+31, height-10-1*(double)height/5.0, 29, 15), Qt::AlignCenter | Qt::AlignVCenter, QString::number(qRound(tmax - (height-1*(double)height/5.0)*(tmax-tmin)/(height))));
        painter.drawText(QRect(width+31, height-10, 29, 15), Qt::AlignCenter | Qt::AlignVCenter, QString::number(qRound(tmax - (height-0*(double)height/5.0)*(tmax-tmin)/(height))));

        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawLine(width+15, height-4*(double)height/5.0, width+35, height-4*(double)height/5.0);
        painter.drawLine(width+15, height-3*(double)height/5.0, width+35, height-3*(double)height/5.0);
        painter.drawLine(width+15, height-2*(double)height/5.0, width+35, height-2*(double)height/5.0);
        painter.drawLine(width+15, height-1*(double)height/5.0, width+35, height-1*(double)height/5.0);
        painter.drawLine(width+15, height-1, width+35, height-1);

        scene.clear();
        scene.addPixmap(QPixmap::fromImage(*image));
    }

    else
        return;


}

QColor VisualizationForm::getPixelColor(int palette, double temp, double tMin, double tMax)
{
    QColor pixelColor;

    /*color palette*/
    if(palette == 1){
        if(tMin + (tMax-tMin)/2.0 == temp)
            pixelColor = QColor(Qt::green);
        if(temp < tMin + (tMax-tMin)/2.0){
            if(temp < tMin || temp > tMax)
                pixelColor = QColor(Qt::yellow);
            else
                pixelColor.setRgbF(0, (temp-tMin)/((tMax-tMin)/2.0), 1 - (temp-tMin)/((tMax-tMin)/2.0));
        }
        if(temp > tMin + (tMax-tMin)/2.0){
            if(temp < tMin || temp > tMax)
                pixelColor = QColor(Qt::yellow);
            else
                pixelColor.setRgbF((temp-tMin)/((tMax-tMin)/2.0) - 1, 2 - (temp-tMin)/((tMax-tMin)/2.0), 0);
        }
    }

    /*white-black palette*/
    if(palette == 2){

        if(temp < tMin || temp > tMax)
            pixelColor = QColor(Qt::yellow);
        else
            pixelColor.setRgbF(1 - (temp-tMin)/((tMax-tMin)), 1 - (temp-tMin)/((tMax-tMin)), 1 - (temp-tMin)/((tMax-tMin)));
    }

    return pixelColor;

}

void VisualizationForm::on_fiPushButton_clicked()
{
    ui->graphicsView->scale(1.0/zoom, 1.0/zoom);

    QRectF imageRect = image->rect();
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
    QString saveName = heatFilenamecsv.remove(".csv");
    if(paletteNum == 1)
        image->save(saveName + "_col.png");
    if(paletteNum == 2)
        image->save(saveName + "_bw.png");
}

void VisualizationForm::on_tempPushButton_clicked()
{
    TemperatureDialog temp;
    connect(&temp, &TemperatureDialog::signalPaletteChanged, this, &VisualizationForm::slotPalletteChanged);
    connect(this, &VisualizationForm::signalSendTminTmax, &temp, &TemperatureDialog::slotTminTmax);
    connect(&temp, &TemperatureDialog::signalSendMinMax, this, &VisualizationForm::slotTempMinMax);
    connect(this, &VisualizationForm::signalSendPalette, &temp, &TemperatureDialog::slotSetPalette);
    emit signalSendTminTmax(tmin, tmax);
    emit signalSendPalette(paletteNum);
    temp.exec();

}

void VisualizationForm::slotTempMinMax(double min, double max)
{
    tmin = min;
    tmax = max;
    drawHeatMap(heatMatrixMap);
}

void VisualizationForm::slotPalletteChanged(int palette)
{
    paletteNum = palette;
}

void VisualizationForm::on_clearPushButton_clicked()
{
    heatMatrixMap.clear();
    scene.clear();
}
