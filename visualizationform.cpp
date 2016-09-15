#include "visualizationform.h"
#include "ui_visualizationform.h"

VisualizationForm::VisualizationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisualizationForm)
{
    ui->setupUi(this);
    connect(this, &VisualizationForm::signalXlsToCsv, this, &VisualizationForm::slotXlsToCsv);
}

VisualizationForm::~VisualizationForm()
{
    delete ui;
}

void VisualizationForm::on_loadPushButton_clicked()
{
    QFile heatFile(QFileDialog::getOpenFileName(this, "Open heat data", "../../", "*.xls"));
    if(!heatFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("baj");
        return;
    }

    QFile heatFilecsv;
    if(heatFile.fileName().endsWith(".xls")){
        QString heatFilenamecsv = QString(heatFile.fileName()).replace(".xls", ".csv");
        if(!QFile(heatFilenamecsv).exists())
            emit signalXlsToCsv(heatFile);
        if(QFile(heatFilenamecsv).exists())
            heatFilecsv.setFileName(heatFilenamecsv);
        else
            return;
    }

    heatFile.close();

    if(!heatFilecsv.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("baj");
        return;
    }

    QTextStream read(&heatFilecsv);
    QString line;
    int count = 1;

    while(!read.atEnd()){
        line = read.readLine();

        if(line.startsWith("0") || line.startsWith("1") || line.startsWith("2") || line.startsWith("3") || line.startsWith("4") || line.startsWith("5") || line.startsWith("6") || line.startsWith("7") || line.startsWith("8") || line.startsWith("9")){
            QStringList lineData = QString(line).split(",");
            heatMatrixMap[count] = lineData;
            count++;
        }
    }

    qDebug("alma");

}

void VisualizationForm::slotXlsToCsv(QFile &file)
{
    QProcess proc;
    QString filename = file.fileName();
    QString filenamecsv(QString(filename).replace(".xls", ".csv"));
    QString cmd("ssconvert " + filename + " " + filenamecsv);
    proc.execute(cmd);
    proc.waitForFinished();
    proc.close();
}
