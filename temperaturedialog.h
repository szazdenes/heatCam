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

public slots:
    void slotTminTmax(double tmin, double tmax);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::TemperatureDialog *ui;
};

#endif // TEMPERATUREDIALOG_H
