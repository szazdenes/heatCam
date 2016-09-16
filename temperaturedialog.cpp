#include "temperaturedialog.h"
#include "ui_temperaturedialog.h"

TemperatureDialog::TemperatureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemperatureDialog)
{
    ui->setupUi(this);
}

TemperatureDialog::~TemperatureDialog()
{
    delete ui;
}

void TemperatureDialog::on_buttonBox_accepted()
{
    emit signalSendMinMax(ui->minDoubleSpinBox->value(), ui->maxDoubleSpinBox->value());
}

void TemperatureDialog::on_buttonBox_rejected()
{
    this->close();
}

void TemperatureDialog::slotTminTmax(double tmin, double tmax)
{
    ui->minDoubleSpinBox->setValue(tmin);
    ui->maxDoubleSpinBox->setValue(tmax);
}
