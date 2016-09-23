#include "analyzationform.h"
#include "ui_analyzationform.h"

AnalyzationForm::AnalyzationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalyzationForm)
{
    ui->setupUi(this);
}

AnalyzationForm::~AnalyzationForm()
{
    delete ui;
}

void AnalyzationForm::slotLineData(QStringList lineData)
{
    QVector<QPointF> dataPoints;
    int count = 1;
    foreach(QString current, lineData){
        QPointF currentPoint((double)count, current.toDouble());
        dataPoints.append(currentPoint);
        count++;
    }

    plotHeatLine(ui->plotWidget, dataPoints);

}

void AnalyzationForm::plotHeatLine(QwtPlot *plot, QVector<QPointF> &data)
{
    QwtPlotCurve *curve = new QwtPlotCurve();

    plot->detachItems();
    plot->setAxisTitle(QwtPlot::xBottom, "Data points");
    plot->setAxisTitle(QwtPlot::yLeft, "Temperature (°C)");
    plot->setCanvasBackground(Qt::white);
    curve->setSamples(data);
    curve->setPen(QPen(Qt::black, 1));
    curve->attach(plot);
    plot->axisAutoScale(QwtPlot::xBottom);
    plot->axisAutoScale(QwtPlot::yLeft);
    plot->replot();
}
