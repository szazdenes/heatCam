 #include "visualizationform.h"
#include "ui_visualizationform.h"

VisualizationForm::VisualizationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisualizationForm)
{
    ui->setupUi(this);

    connect(this, &VisualizationForm::signalXlsToCsv, this, &VisualizationForm::slotXlsToCsv);
    connect(ui->graphicsView, &ImageGraphicsviewForm::signalMouseMoved, this, &VisualizationForm::slotMouseMoved);
    connect(ui->graphicsView, &ImageGraphicsviewForm::signalLeftButtonPressed, this, &VisualizationForm::slotMouseButtonPressed);
    connect(ui->graphicsView, &ImageGraphicsviewForm::signalLeftButtonReleased, this, &VisualizationForm::slotMouseButtonReleased);
    connect(ui->graphicsView, &ImageGraphicsviewForm::signalWheelUp, this, &VisualizationForm::slotWheelUp);
    connect(ui->graphicsView, &ImageGraphicsviewForm::signalWheelDown, this, &VisualizationForm::slotWheelDown);

    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    zoom = 1;
    tmin = 0;
    tmax = 60;
    paletteNum = 2;
    pensize = 50;
    imageLoaded = false;
    isButtonPressed = false;
    isHeatLineOn = false;

    ui->lineCheckBox->setChecked(true);
    openFileName = "../../";

//    rapidEvaluation();
}

VisualizationForm::~VisualizationForm()
{
    imageLoaded = false;
    delete ui;
}

void VisualizationForm::on_loadPushButton_clicked()
{
    if(imageLoaded == true){
        imageLoaded = false;
    }

    heatMatrixMap.clear();

    openFileName = QFileDialog::getOpenFileName(this, "Open heat data", openFileName, "*.xls");
    emit signalSendFilename(openFileName);

    QFile heatFile(openFileName);
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

    ui->fileNameLineEdit->setText(heatFilenamecsv.split(".").first());
    drawHeatMap(heatMatrixMap);
    refreshMask();

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
        imageLoaded = false;
        int width = heatMap[heatMap.keys().first()].size();
        int height = heatMap.keys().size();
        image = QImage(width+60, height+5, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::white);
        mask = QImage(image.width(), image.height(), QImage::Format_ARGB32_Premultiplied);
        mainImage = QImage(image.width(), image.height(), QImage::Format_ARGB32_Premultiplied);

        for(int w = 0; w < width; w++){
            for(int h = 0; h < height; h++){
                QColor pixColor;
                if(!heatMap[h+1].isEmpty()){
                    double temp = QString(heatMap[h+1].at(w)).toDouble();
                    pixColor = getPixelColor(paletteNum, temp, tmin, tmax);
                    image.setPixelColor(w, h, pixColor);
                }
            }
        }

        imageLoaded = true;
        QPainter painter(&image);



        for(int w = width+2; w <= width+28; w++){
            for(int h = 0; h < height; h++){
                QColor pixColor;
                double temp = tmax - h*(tmax-tmin)/(height-1);
                pixColor = getPixelColor(paletteNum, temp, tmin, tmax);
                image.setPixelColor(w, h, pixColor);
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

        mainImage = image;
        maskedImage = image;
        scene.clear();
        scene.addPixmap(QPixmap::fromImage(mainImage));
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
                pixelColor = QColor(Qt::magenta);
            else
                pixelColor.setRgbF(0, (temp-tMin)/((tMax-tMin)/2.0), 1 - (temp-tMin)/((tMax-tMin)/2.0));
        }
        if(temp > tMin + (tMax-tMin)/2.0){
            if(temp < tMin || temp > tMax)
                pixelColor = QColor(Qt::magenta);
            else
                pixelColor.setRgbF((temp-tMin)/((tMax-tMin)/2.0) - 1, 2 - (temp-tMin)/((tMax-tMin)/2.0), 0);
        }
    }

    /*white-black palette*/
    if(palette == 2){

        if(temp < tMin || temp > tMax)
            pixelColor = QColor(Qt::magenta);
        else
            pixelColor.setRgbF(1 - (temp-tMin)/((tMax-tMin)), 1 - (temp-tMin)/((tMax-tMin)), 1 - (temp-tMin)/((tMax-tMin)));
    }

    return pixelColor;

}

void VisualizationForm::rapidEvaluation()
{

    QStringList filenames = QFileDialog::getOpenFileNames(this, "Open heat data", "../../", "*.xls");

    foreach(QString heatfilename, filenames){
        heatMatrixMap.clear();
        QFile heatFile(heatfilename);
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

        tmin = 10;
        tmax = 60;

        paletteNum = 1;
        drawHeatMap(heatMatrixMap);
        ui->savePushButton->click();
        paletteNum = 2;
        drawHeatMap(heatMatrixMap);
        ui->savePushButton->click();

    }
}

void VisualizationForm::refreshImageMask()
{
    QPainter painter(&mainImage);
    QPainter painter2(&maskedImage);

    if(isButtonPressed){
        painter.drawImage(0, 0, maskedImage);
        painter.drawImage(0, 0, mask);
        painter.end();
        scene.clear();
        scene.addPixmap(QPixmap::fromImage(mainImage));
    }
    else{
        painter2.drawImage(0,0, mainImage);
        painter2.end();
        scene.clear();
        scene.addPixmap(QPixmap::fromImage(mainImage));
    }

    isHeatLineOn = true;
}

void VisualizationForm::refreshHeatlineMask(QPoint startPos, QPoint endPos)
{
    QPainter painter(&mask);
    QPen pen;
    pen.setColor(Qt::yellow);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawLine(startPos, endPos);
    painter.end();
}

void VisualizationForm::setCursorImage(double size)
{
    if(ui->areaCheckBox->isChecked()){
        QImage cursorImage = QImage(size, size, QImage::Format_ARGB32);
        QPainter cursorPainter(&cursorImage);
        cursorPainter.setCompositionMode(QPainter::CompositionMode_Source);
        cursorPainter.fillRect(cursorImage.rect(), Qt::transparent);
        cursorPainter.setPen(Qt::yellow);
        cursorPainter.drawEllipse(0, 0, size, size);
        cursorPainter.end();
        cursor = QCursor(QPixmap::fromImage(cursorImage));
        ui->graphicsView->setCursor(cursor);
    }
    else
        return;
}

void VisualizationForm::refreshMask()
{
    mask.fill(Qt::transparent);
}

void VisualizationForm::paintMask(QPointF pos)
{
    QPainter painter(&mask);
    QBrush brush;
    brush.setColor(Qt::yellow);
    brush.setStyle(Qt::SolidPattern);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(pos.x() - pensize/zoom/2.0, pos.y() - pensize/zoom/2.0, pensize/zoom, pensize/zoom);
    painter.end();

    mainImage = QImage(image.width(), image.height(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter2(&mainImage);
    painter2.drawImage(0, 0, image);
    painter2.setOpacity(0.5);
    painter2.drawImage(0, 0, mask);
    painter2.end();
    scene.clear();
    scene.addPixmap(QPixmap::fromImage(mainImage));
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
    if(isHeatLineOn == false){
        QString saveName = heatFilenamecsv.remove(".csv");
        if(paletteNum == 1)
            image.save(saveName + "_col.png");
        if(paletteNum == 2)
            image.save(saveName + "_bw.png");
    }

    if(isHeatLineOn == true){
        QString saveName = heatFilenamecsv.remove(".csv");
        if(paletteNum == 1)
            mainImage.save(saveName + "_col_line.png");
        if(paletteNum == 2)
            mainImage.save(saveName + "_bw_line.png");
    }
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
    mask.fill(Qt::transparent);
    image.fill(Qt::transparent);
    mainImage.fill(Qt::transparent);
    scene.clear();
}

void VisualizationForm::slotMouseMoved(QPointF pos)
{
    if(imageLoaded == true){
        if(qRound(pos.x()) >= 0 && qRound(pos.y()) >=0 && qRound(pos.x()) <= image.width() && qRound(pos.y()) <= image.height()){
            if(!heatMatrixMap[qRound(pos.y())].isEmpty() && qRound(pos.y()) < heatMatrixMap.keys().size() && qRound(pos.x()) < heatMatrixMap[qRound(pos.y())].size()){
                QToolTip::showText(QCursor::pos(), "T=" + heatMatrixMap[qRound(pos.y())].at(qRound(pos.x())) + " °C", ui->graphicsView);
                if(ui->lineCheckBox->isChecked()){
                    if(isButtonPressed == true){
                        refreshMask();
                        refreshHeatlineMask(start, QPoint(qRound(pos.x()), qRound(pos.y())));
                        refreshImageMask();
                    }
                }
                if(ui->areaCheckBox->isChecked()){
                    setCursorImage(pensize);
                    if(isButtonPressed == true){
                        paintMask(pos);
                    }

                }
            }
        }

    }
}

void VisualizationForm::slotMouseButtonPressed(QPointF pos)
{
    if(ui->lineCheckBox->isChecked()){
        start = QPoint(qRound(pos.x()), qRound(pos.y()));
        isButtonPressed = true;
    }

    if(ui->areaCheckBox->isChecked()){
        isButtonPressed = true;
    }
}

void VisualizationForm::slotMouseButtonReleased(QPointF pos)
{
    if(ui->lineCheckBox->isChecked()){
        end = QPoint(qRound(pos.x()), qRound(pos.y()));
        isButtonPressed = false;
        refreshMask();
        refreshHeatlineMask(start, QPoint(qRound(pos.x()), qRound(pos.y())));
        refreshImageMask();
        getDataFromHeatLine();
    }
    if(ui->areaCheckBox->isChecked()){
        isButtonPressed = false;
    }
}

void VisualizationForm::slotWheelUp()
{
    pensize+=5;
    setCursorImage(pensize);
}

void VisualizationForm::slotWheelDown()
{
    pensize-=5;
    setCursorImage(pensize);
}

void VisualizationForm::getDataFromHeatLine()
{
    QStringList lineData;
    if(start != end){
        int minX = qMin(start.x(), end.x());
        int maxX = qMax(start.x(), end.x());
        int minY = qMin(start.y(), end.y());
        int maxY = qMax(start.y(), end.y());

        if(minX == maxX && minY != maxY){
            int i = minX;
            for(int j = minY; j <= maxY; j++){
                if(mainImage.pixelColor(i, j) == Qt::yellow)
                    lineData.append(heatMatrixMap[j].at(i));
            }
        }
        else if(minX != maxX && minY == maxY){
            int j = minY;
            for(int i = minX; i <= maxX; i++){
                if(mainImage.pixelColor(i, j) == Qt::yellow)
                    lineData.append(heatMatrixMap[j].at(i));
            }
        }
        else{
            for(int i = minX; i <= maxX; i++){
                for(int j = minY; j <= maxY; j++){
                    if(mainImage.pixelColor(i, j) == Qt::yellow)
                        lineData.append(heatMatrixMap[j].at(i));
                }
            }
        }
    }
    if(!lineData.isEmpty())
        emit signalSendLineData(lineData);

//    foreach(QString data, lineData)
//        qDebug() << data;
}

void VisualizationForm::on_clearMaskPushButton_clicked()
{
    refreshMask();
    mainImage = image;
    refreshImageMask();
}

void VisualizationForm::on_lineCheckBox_toggled(bool checked)
{
    if(checked == false)
        emit signalLineOff();
    else{
        emit signalLineOn();
        ui->areaCheckBox->setChecked(false);
        ui->graphicsView->setCursor(QCursor(Qt::ArrowCursor));
        refreshMask();
    }
}

void VisualizationForm::on_areaCheckBox_toggled(bool checked)
{
    if(checked == true){
        ui->lineCheckBox->setChecked(false);
        refreshMask();
    }
}

void VisualizationForm::on_areaToTablePushButton_clicked()
{
    QStringList areaData;

    if(ui->areaCheckBox->isChecked()){
        for(int i = 0; i < mask.width(); i++){
            for(int j = 0; j < mask.height(); j++){
                if(mask.pixelColor(i, j) == Qt::yellow)
                    areaData.append(heatMatrixMap[j].at(i));
            }
        }
        if(!areaData.isEmpty())
            emit signalSendAreaData(areaData, QString(heatFilenamecsv).remove(".csv"));
    }
    else
        return;
}
