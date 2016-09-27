#include "analyzationform.h"
#include "ui_analyzationform.h"

AnalyzationForm::AnalyzationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalyzationForm)
{
    ui->setupUi(this);
    ui->kernelSpinBox->setValue(10);
    ui->sigmaSpinBox->setValue(5);

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

QVector<QPointF> AnalyzationForm::gaussianSmooth(QVector<QPointF> data, int half_kernel, double sigma)
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
    int surroundings = 5;

    foreach(QPointF current, dataList)
        tempList.append(current.y());

    qSort(tempList);
    if(qAbs(tempList.last()-tempList.first()) > 1){
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
    QList<QPointF> minList, maxList;

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
    int diff = 10;

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
