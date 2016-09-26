#include "analyzationform.h"
#include "ui_analyzationform.h"

AnalyzationForm::AnalyzationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalyzationForm)
{
    ui->setupUi(this);
//    curve1 = new QwtPlotCurve();
//    curve2 = new QwtPlotCurve();
}

AnalyzationForm::~AnalyzationForm()
{
    delete ui;
}

void AnalyzationForm::slotLineData(QStringList lineData)
{
    dataPoints.clear();
    int count = 1;
    foreach(QString current, lineData){
        QPointF currentPoint((double)count, current.toDouble());
        dataPoints.append(currentPoint);
        count++;
    }

    plotHeatLine(ui->plotWidget, dataPoints);
    ui->fromHorizontalSlider->setMinimum(0);
    ui->toHorizontalSlider->setMaximum(dataPoints.size()-1);
    ui->fromHorizontalSlider->setValue(ui->fromHorizontalSlider->minimum());
    ui->toHorizontalSlider->setValue(ui->toHorizontalSlider->maximum());

}

void AnalyzationForm::plotHeatLine(QwtPlot *plot, QVector<QPointF> &data)
{
    QwtPlotCurve *curve1 = new QwtPlotCurve();

    plot->detachItems();
    plot->setAxisTitle(QwtPlot::xBottom, "Data points");
    plot->setAxisTitle(QwtPlot::yLeft, "Temperature (°C)");
    plot->setCanvasBackground(Qt::white);
    curve1->setSamples(data);
    curve1->setPen(QPen(Qt::black, 1));
    curve1->attach(plot);
    plot->setAxisAutoScale(QwtPlot::xBottom, true);
    plot->setAxisAutoScale(QwtPlot::yLeft, true);
    plot->axisAutoScale(QwtPlot::xBottom);
    plot->axisAutoScale(QwtPlot::yLeft);
    plot->replot();
}

void AnalyzationForm::rangeSelection(int from, int to)
{
    if(!dataPoints.isEmpty()){
        QVector<QPointF> selectedData;
        for(int i = from; i <= to; i++)
            selectedData.append(dataPoints.at(i));

        plotSelectedRange(ui->plotWidget, selectedData);
    }

}

void AnalyzationForm::plotSelectedRange(QwtPlot *plot, QVector<QPointF> &data)
{
    QwtPlotCurve *curve1 = new QwtPlotCurve();
    QwtPlotCurve *curve2 = new QwtPlotCurve();

    plot->detachItems();
    curve1->setSamples(dataPoints);
    curve1->setPen(QPen(Qt::black, 1));
    curve1->attach(plot);
    curve2->setSamples(data);
    curve2->setPen(QPen(Qt::red, 2));
    curve2->attach(plot);
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

QPair<QPointF, QPointF> AnalyzationForm::minMaxPosition(QList<QPointF> &dataList)
{
    QPair<QPointF, QPointF> minmax = QPair<QPointF, QPointF>(QPointF(0,0), QPointF(0,0));
    QList<double> tempList;
    double min, max;
    int surroundings = 1;

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
    QList<QPointF> minmaxList;
    for(int j = 0; j < dataPoints.size() - qAbs(to-from); j += 1){
        rangeList.clear();
        for(int i = 0 + j; i <= qAbs(to-from) + j; i++){
            rangeList.append(dataPoints.at(i));
        }
        QPair<QPointF, QPointF> minmax = minMaxPosition(rangeList);
        if(!minmaxList.contains(minmax.first) && minmax.first != QPointF(0,0))
            minmaxList.append(minmax.first);
        if(!minmaxList.contains(minmax.second) && minmax.second != QPointF(0,0))
            minmaxList.append(minmax.second);
    }
    for(int i = 0; i < minmaxList.size(); i++){
        for(int j = 0; j < minmaxList.size(); j++){
            if(qAbs(minmaxList.at(i).x() - minmaxList.at(j).x()) < 1)
                minmaxList.removeAt(j);
        }
    }
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
