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


    QwtPlotCurve curve;
    QFont font;
    font.setBold(true);
    font.setPointSize(10);

    ui->plotWidget->detachItems();
    ui->plotWidget->setAxisTitle(QwtPlot::xBottom, "Data points");
    ui->plotWidget->setAxisTitle(QwtPlot::yLeft, "Temperature (°C)");
    curve.setSamples(dataPoints);
    curve.setPen(QPen(Qt::black, 1));
    curve.attach(ui->plotWidget);
    ui->plotWidget->axisAutoScale(QwtPlot::xBottom);
    ui->plotWidget->axisAutoScale(QwtPlot::yLeft);
    ui->plotWidget->setCanvasBackground(Qt::white);
//    ui->plotWidget->replot();
}
