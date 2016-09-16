#ifndef TEMPERATUREDIALOG_H
#define TEMPERATUREDIALOG_H

#include <QDialog>

namespace Ui {
class TemperatureDialog;
}

class TemperatureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TemperatureDialog(QWidget *parent = 0);
    ~TemperatureDialog();

signals:
    void signalSendMinMax(double min, double max);
    void signalPaletteChanged(int palette);

public slots:
    void slotTminTmax(double tmin, double tmax);
    void slotSetPalette(int palette);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_minDoubleSpinBox_valueChanged(double arg1);

    void on_maxDoubleSpinBox_valueChanged(double arg1);

private:
    Ui::TemperatureDialog *ui;
    int palette;
};

#endif // TEMPERATUREDIALOG_H
