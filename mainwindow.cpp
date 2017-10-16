#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->visualizeWidget, &VisualizationForm::signalSendLineData, ui->analyzewidget, &AnalyzationForm::slotLineData);
    connect(ui->visualizeWidget, &VisualizationForm::signalLineOn, ui->analyzewidget, &AnalyzationForm::slotLineOn);
    connect(ui->visualizeWidget, &VisualizationForm::signalLineOff, ui->analyzewidget, &AnalyzationForm::slotLineOff);
    connect(ui->visualizeWidget, &VisualizationForm::signalSendAreaData, ui->analyzewidget,  &AnalyzationForm::slotAreaData);
    connect(ui->visualizeWidget, &VisualizationForm::signalSendFilename, ui->analyzewidget, &AnalyzationForm::slotFileName);
    connect(ui->analyzewidget, &AnalyzationForm::signalSaveHeatMap, ui->visualizeWidget, &VisualizationForm::on_savePushButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}
