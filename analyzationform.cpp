#include "analyzationform.h"
#include "ui_analyzationform.h"

AnalyzationForm::AnalyzationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalyzationForm)
{
    ui->setupUi(this);
    ui->sendTablePushButton->setDisabled(true);
    ui->exportPushButton->setDisabled(true);
    ui->exportPlotpushButton->setDisabled(true);

    ui->kernelSpinBox->setValue(10);
    ui->sigmaSpinBox->setValue(5);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Tmax (°C)" << "Tmin (°C)" << "delta T");

}

AnalyzationForm::~AnalyzationForm()
{
    delete ui;
}

void AnalyzationForm::slotLineData(QStringList lineData)
{
    dataPoints.clear();
    smoothedDataPoints.clear();
    int count = 1;
    foreach(QString current, lineData){
        QPointF currentPoint((double)count, current.toDouble());
        dataPoints.append(currentPoint);
        count++;
    }

    smoothedDataPoints = gaussianSmooth(dataPoints, ui->kernelSpinBox->value(), (double) ui->sigmaSpinBox->value());
    plotHeatLine(ui->plotWidget, smoothedDataPoints);
    ui->fromHorizontalSlider->setMinimum(0);
    ui->toHorizontalSlider->setMaximum(dataPoints.size()-1);
    ui->fromHorizontalSlider->setValue(ui->fromHorizontalSlider->minimum());
    ui->toHorizontalSlider->setValue(ui->toHorizontalSlider->maximum());

}

void AnalyzationForm::plotHeatLine(QwtPlot *plot, QVector<QPointF> &data)
{
    QwtPlotCurve *curve1 = new QwtPlotCurve();
    QwtPlotCurve *curve2 = new QwtPlotCurve();

    plot->detachItems();
    plot->setAxisTitle(QwtPlot::xBottom, "Data points");
    plot->setAxisTitle(QwtPlot::yLeft, "Temperature (°C)");
    plot->setCanvasBackground(Qt::white);
    curve1->setSamples(data);
    curve1->setPen(QPen(Qt::blue, 1));
    curve1->attach(plot);
    curve2->setSamples(dataPoints);
    curve2->setPen(QPen(Qt::black, 1));
    curve2->attach(plot);
    plot->setAxisAutoScale(QwtPlot::xBottom, true);
    plot->setAxisAutoScale(QwtPlot::yLeft, true);
    plot->axisAutoScale(QwtPlot::xBottom);
    plot->axisAutoScale(QwtPlot::yLeft);
    plot->replot();
}

void AnalyzationForm::rangeSelection(int from, int to)
{
    if(!smoothedDataPoints.isEmpty()){
        QVector<QPointF> selectedData;
        for(int i = from; i <= to; i++)
            selectedData.append(smoothedDataPoints.at(i));

        plotSelectedRange(ui->plotWidget, selectedData);
    }

}

void AnalyzationForm::plotSelectedRange(QwtPlot *plot, QVector<QPointF> &data)
{
    QwtPlotCurve *curve1 = new QwtPlotCurve();
    QwtPlotCurve *curve2 = new QwtPlotCurve();
    QwtPlotCurve *curve3 = new QwtPlotCurve();

    plot->detachItems();
    curve1->setSamples(smoothedDataPoints);
    curve1->setPen(QPen(Qt::blue, 1));
    curve1->attach(plot);
    curve2->setSamples(data);
    curve2->setPen(QPen(Qt::red, 2));
    curve2->attach(plot);
    curve3->setSamples(dataPoints);
    curve3->setPen(QPen(Qt::black, 1));
    curve3->attach(plot);
    plot->setAxisAutoScale(QwtPlot::xBottom, false);
    plot->setAxisAutoScale(QwtPlot::yLeft, false);
    plot->replot();
}

void AnalyzationForm::plotMinMax(QwtPlot *plot, QList<QPointF> &data)
{
    foreach(QPointF currentPoint, data){
        QwtPlotMarker* marker = new QwtPlotMarker();
        QwtSymbol* sym = new QwtSymbol();
        sym->setStyle(QwtSymbol::Diamond);
        sym->setSize(10,10);
        sym->setColor(Qt::green);
        marker->setSymbol(sym);
        marker->setValue(currentPoint);
        marker->attach(plot);
        plot->replot();
    }
}

double AnalyzationForm::getAverage(QList<double> &list)
{
    double result;
    for (int i = 0; i < list.size(); i++)
        result += (double)list.at(i) / (double)list.size();
    return result;
}

double AnalyzationForm::getStD(QList<double> &list)
{
    double result;
    double average = getAverage(list);
    for (int i = 0; i < list.size(); i++)
        result += (((double)list.at(i) - average)*((double)list.at(i) - average)) / (double)list.size();
    result = qSqrt(result);
    return result;
}

QVector<QPointF> AnalyzationForm::gaussianSmooth(QVector<QPointF> &data, int half_kernel, double sigma)
{
    QVector<QPointF> smoothedData;
    double smoothed;
    for(int i = 0; i < data.size(); i++){
        double weight = 0, sum = 0, weightSum = 0;
        for(int j = -half_kernel; j <= half_kernel; j++){
            if(i+j >= 0 && i+j < data.size()){
                weight = qExp(-(j*j)/(2*sigma*sigma));
                sum += data.at(i+j).y() * weight;
            }
            if(i+j < 0 || i+j >= data.size()){
                weight = 0;
                sum += weight;
            }
            weightSum += weight;
        }
        smoothed = sum / weightSum;
        smoothedData.append(QPointF(data.at(i).x(), smoothed));
    }

    return smoothedData;
}

QPair<QPointF, QPointF> AnalyzationForm::minMaxPosition(QList<QPointF> &dataList)
{
    QPair<QPointF, QPointF> minmax = QPair<QPointF, QPointF>(QPointF(0,0), QPointF(0,0));
    QList<double> tempList;
    double min, max;
    int surroundings = 3;

    foreach(QPointF current, dataList)
        tempList.append(current.y());

    qSort(tempList);
    if(qAbs(tempList.last()-tempList.first()) > 0.2){
        min = tempList.first();
        max = tempList.last();
        QList<QPointF> minList, maxList;

        while(minmax.first == QPointF(0,0)){
            foreach(QPointF current, dataList){
                if(current.y() == min)
                    minList.append(current);
            }
            foreach(QPointF current, minList){
                if(dataList.indexOf(current) - surroundings >= 0 && dataList.indexOf(current) + surroundings < dataList.size()  && dataList.at(dataList.indexOf(current) - surroundings).y() > min && dataList.at(dataList.indexOf(current) + surroundings).y() > min)
                    minmax.first = current;
            }

            minList.clear();
            if(!tempList.isEmpty())
                tempList.removeFirst();
            if(!tempList.isEmpty())
                min = tempList.first();
            else
                break;

        }
        while(minmax.second == QPointF(0,0)){
            foreach(QPointF current, dataList){
                if(current.y() == max)
                    maxList.append(current);
            }
            foreach(QPointF current, maxList){
                if(dataList.indexOf(current) - surroundings >= 0 && dataList.indexOf(current) + surroundings < dataList.size()  && dataList.at(dataList.indexOf(current) - surroundings).y() < max && dataList.at(dataList.indexOf(current) + surroundings).y() < max)
                    minmax.second = current;
            }

            maxList.clear();
            if(!tempList.isEmpty())
                tempList.removeLast();
            if(!tempList.isEmpty())
                max = tempList.last();
            else
                break;
        }
    }
    else
        return QPair<QPointF, QPointF>(QPointF(0,0), QPointF(0,0));

    return minmax;
}

void AnalyzationForm::on_minMaxPushButton_clicked()
{
    int from = ui->fromHorizontalSlider->value();
    int to = ui->toHorizontalSlider->value();

    QList<QPointF> rangeList;
    minList.clear(); maxList.clear();

    for(int j = 0; j < smoothedDataPoints.size() - qAbs(to-from); j += 1){
        rangeList.clear();
        for(int i = 0 + j; i <= qAbs(to-from) + j; i++){
            rangeList.append(smoothedDataPoints.at(i));
        }
        QPair<QPointF, QPointF> minmax = minMaxPosition(rangeList);
        if(!minList.contains(minmax.first) && minmax.first != QPointF(0,0))
            minList.append(minmax.first);
        if(!maxList.contains(minmax.second) && minmax.second != QPointF(0,0))
            maxList.append(minmax.second);
    }

    /*remove points very close to each other*/
    int diff = 5;

    for(int i = 0; i < minList.size()-1; i++){
        if(qAbs(minList.at(i).x() - minList.at(i+1).x()) < diff){
            if(minList.at(i).y() < minList.at(i+1).y())
                minList.removeAt(i+1);
            else
                minList.removeAt(i);
            i--;
        }
    }

    for(int i = 0; i < maxList.size()-1; i++){
        if(qAbs(maxList.at(i).x() - maxList.at(i+1).x()) < diff){
            if(maxList.at(i).y() < maxList.at(i+1).y())
                maxList.removeAt(i);
            else
                maxList.removeAt(i+1);
            i--;
        }
    }

    QList<QPointF> minmaxList;
    minmaxList.append(minList);
    minmaxList.append(maxList);
    plotMinMax(ui->plotWidget, minmaxList);

    ui->sendTablePushButton->setEnabled(true);
    ui->exportPlotpushButton->setEnabled(true);
}

void AnalyzationForm::on_fromHorizontalSlider_valueChanged(int value)
{
    ui->toHorizontalSlider->setMinimum(value);
    rangeSelection(value, ui->toHorizontalSlider->value());
}

void AnalyzationForm::on_toHorizontalSlider_valueChanged(int value)
{
    ui->fromHorizontalSlider->setMaximum(value);
    rangeSelection(ui->fromHorizontalSlider->value(), value);
}

void AnalyzationForm::on_sendTablePushButton_clicked()
{
    while(ui->tableWidget->rowCount() != 0)
        ui->tableWidget->removeRow(ui->tableWidget->rowCount()-1);

    QList<double> deltaList;
    for(int i = 0; i < qMin(minList.size(), maxList.size()); i++){
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(QString::number(maxList.at(i).y())));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(QString::number(minList.at(i).y())));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 2, new QTableWidgetItem(QString::number(qAbs(maxList.at(i).y() - minList.at(i).y()))));
        deltaList.append(qAbs(maxList.at(i).y() - minList.at(i).y()));
    }

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(QString("Average")));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(QString("")));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 2, new QTableWidgetItem(QString::number(getAverage(deltaList))));

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(QString("StD")));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(QString("")));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 2, new QTableWidgetItem(QString::number(getStD(deltaList))));

    ui->exportPushButton->setEnabled(true);

}

void AnalyzationForm::on_kernelSpinBox_valueChanged(int arg1)
{
    smoothedDataPoints = gaussianSmooth(dataPoints, arg1, (double) ui->sigmaSpinBox->value());
    rangeSelection(ui->fromHorizontalSlider->value(), ui->toHorizontalSlider->value());
}

void AnalyzationForm::on_sigmaSpinBox_valueChanged(int arg1)
{
    smoothedDataPoints = gaussianSmooth(dataPoints, ui->kernelSpinBox->value(), (double) arg1);
    rangeSelection(ui->fromHorizontalSlider->value(), ui->toHorizontalSlider->value());
}

void AnalyzationForm::on_exportPushButton_clicked()
{
    QFile exportFile(QFileDialog::getSaveFileName(this, "Save table data", "../../"));
    if(!exportFile.fileName().endsWith("_table.csv"))
        exportFile.setFileName(exportFile.fileName() + "_table.csv");
    if(!exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug("baj");
        return;
    }

    QTextStream out(&exportFile);
    out << ui->tableWidget->horizontalHeaderItem(0)->text() << "\t" << ui->tableWidget->horizontalHeaderItem(1)->text() << "\t" << ui->tableWidget->horizontalHeaderItem(2)->text() << "\n";

    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        QString second = !ui->tableWidget->item(i, 1)->text().isEmpty() ? ui->tableWidget->item(i, 1)->text() : "";
        out << ui->tableWidget->item(i, 0)->text() << "\t" << second << "\t" << ui->tableWidget->item(i, 2)->text() << "\n";
    }

    exportFile.close();
}

void AnalyzationForm::on_exportPlotpushButton_clicked()
{
    QFile exportPlotFile(QFileDialog::getSaveFileName(this, "Save plot data", "../../"));
    if(!exportPlotFile.fileName().endsWith("_plot.csv"))
        exportPlotFile.setFileName(exportPlotFile.fileName() + "_plot.csv");
    if(!exportPlotFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug("baj");
        return;
    }

    QTextStream out(&exportPlotFile);
    out << "#num" << "\t" << "temp" << "\t" << "smoothed temp" << "\t" << "min" << "\t" << "max" << "\t" << "kernel: " + QString::number(ui->kernelSpinBox->text().toDouble() * 2) << "\t" << "sigma: " + ui->sigmaSpinBox->text() << "\n";

    for(int i = 0; i < dataPoints.size(); i++){
        QString min = "";
        QString max = "";
        foreach(QPointF currentMin, minList){
            if(currentMin.x() == smoothedDataPoints.at(i).x()){
                min = QString::number(currentMin.y());
                break;
            }
        }
        foreach(QPointF currentMax, maxList){
            if(currentMax.x() == smoothedDataPoints.at(i).x()){
                max = QString::number(currentMax.y());
                break;
            }
        }
        out << dataPoints.at(i).x() << "\t" << dataPoints.at(i).y() << "\t" << smoothedDataPoints.at(i).y() << "\t" << min << "\t" << max << "\n";
    }

    exportPlotFile.close();
}

void AnalyzationForm::slotLineOn()
{
    ui->minMaxPushButton->setEnabled(true);
    ui->fromHorizontalSlider->setEnabled(true);
    ui->toHorizontalSlider->setEnabled(true);
    ui->kernelSpinBox->setEnabled(true);
    ui->sigmaSpinBox->setEnabled(true);
    ui->plotWidget->setEnabled(true);
    ui->sendTablePushButton->setDisabled(true);
    ui->exportPushButton->setDisabled(true);
    ui->exportPlotpushButton->setDisabled(true);
}

void AnalyzationForm::slotLineOff()
{
    ui->minMaxPushButton->setDisabled(true);
    ui->exportPlotpushButton->setDisabled(true);
    ui->fromHorizontalSlider->setDisabled(true);
    ui->toHorizontalSlider->setDisabled(true);
    ui->kernelSpinBox->setDisabled(true);
    ui->sigmaSpinBox->setDisabled(true);
    ui->plotWidget->setDisabled(true);
    ui->sendTablePushButton->setDisabled(true);
}
