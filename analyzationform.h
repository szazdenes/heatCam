#ifndef ANALYZATIONFORM_H
#define ANALYZATIONFORM_H

#include <QWidget>
#include <QtMath>
#include <QTextStream>
#include <QFileDialog>
#include <qwt/src/qwt_plot.h>
#include <qwt/src/qwt_plot_curve.h>
#include <qwt/src/qwt_plot_marker.h>
#include <qwt/src/qwt_symbol.h>

namespace Ui {
class AnalyzationForm;
}

class AnalyzationForm : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyzationForm(QWidget *parent = 0);
    ~AnalyzationForm();

public slots:
    void slotLineData(QStringList lineData);
    void slotAreaData(QStringList areaData, QString filename);
    void slotLineOn();
    void slotLineOff();

private slots:
    void on_minMaxPushButton_clicked();

    void on_fromHorizontalSlider_valueChanged(int value);

    void on_toHorizontalSlider_valueChanged(int value);

    void on_sendTablePushButton_clicked();

    void on_kernelSpinBox_valueChanged(int arg1);


    void on_sigmaSpinBox_valueChanged(int arg1);

    void on_exportPushButton_clicked();

    void on_exportPlotpushButton_clicked();

private:
    Ui::AnalyzationForm *ui;
    void plotHeatLine(QwtPlot *plot, QVector<QPointF> &data);
    void rangeSelection(int from, int to);
    void plotSelectedRange(QwtPlot *plot, QVector<QPointF> &data);
    void plotMinMax(QwtPlot *plot, QList<QPointF> &data);
    double getAverage(QList<double> &list);
    double getStD(QList<double> &list);
    QPair<QPointF, QPointF> minMaxPosition(QList<QPointF> &dataList); //first: min, second: max
    QVector<QPointF> gaussianSmooth(QVector<QPointF> &data, int half_kernel, double sigma);

    QVector<QPointF> dataPoints;
    QVector<QPointF> smoothedDataPoints;
    QList<QPointF> minList, maxList;


};

#endif // ANALYZATIONFORM_H
