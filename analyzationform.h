#ifndef ANALYZATIONFORM_H
#define ANALYZATIONFORM_H

#include <QWidget>
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


private slots:
    void on_minMaxPushButton_clicked();

    void on_fromHorizontalSlider_valueChanged(int value);

    void on_toHorizontalSlider_valueChanged(int value);

    void on_sendTablePushButton_clicked();

private:
    Ui::AnalyzationForm *ui;
    void plotHeatLine(QwtPlot *plot, QVector<QPointF> &data);
    void rangeSelection(int from, int to);
    void plotSelectedRange(QwtPlot *plot, QVector<QPointF> &data);
    void plotMinMax(QwtPlot *plot, QList<QPointF> &data);
    QVector<QPointF> dataPoints;
    QPair<QPointF, QPointF> minMaxPosition(QList<QPointF> &dataList); //first: min, second: max

};

#endif // ANALYZATIONFORM_H
