#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->visualizeWidget, &VisualizationForm::signalSendLineData, ui->analyzewidget, &AnalyzationForm::slotLineData);
}

MainWindow::~MainWindow()
{
    delete ui;
}
