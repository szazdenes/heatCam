#ifndef ANALYZATIONFORM_H
#define ANALYZATIONFORM_H

#include <QWidget>
#include <qwt/src/qwt_plot.h>
#include <qwt/src/qwt_plot_curve.h>

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


private:
    Ui::AnalyzationForm *ui;
    void plotHeatLine(QwtPlot *plot, QVector<QPointF> &data);

};

#endif // ANALYZATIONFORM_H
