#include "temperaturedialog.h"
#include "ui_temperaturedialog.h"

TemperatureDialog::TemperatureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemperatureDialog)
{
    ui->setupUi(this);
    ui->minDoubleSpinBox->setValue(0.0);
    ui->maxDoubleSpinBox->setValue(60.0);
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
