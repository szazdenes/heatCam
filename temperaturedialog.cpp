#include "temperaturedialog.h"
#include "ui_temperaturedialog.h"

TemperatureDialog::TemperatureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemperatureDialog)
{
    ui->setupUi(this);
    ui->comboBox->addItems(QStringList() << "color palette" << "white-black palette");
    palette = 1;
}

TemperatureDialog::~TemperatureDialog()
{
    delete ui;
}

void TemperatureDialog::on_buttonBox_accepted()
{
    if(ui->comboBox->currentText() == "color palette")
        palette = 1;
    if(ui->comboBox->currentText() == "white-black palette")
        palette = 2;
    emit signalPaletteChanged(palette);
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

void TemperatureDialog::slotSetPalette(int palette)
{
    if(palette == 1)
        ui->comboBox->setCurrentText("color palette");
    if(palette == 2)
        ui->comboBox->setCurrentText("white-black palette");
}

void TemperatureDialog::on_minDoubleSpinBox_valueChanged(double arg1)
{
    if(arg1 >= ui->maxDoubleSpinBox->value())
        ui->maxDoubleSpinBox->setValue(arg1+1);
}

void TemperatureDialog::on_maxDoubleSpinBox_valueChanged(double arg1)
{
    if(arg1 <= ui->minDoubleSpinBox->value())
        ui->minDoubleSpinBox->setValue(arg1-1);
}
