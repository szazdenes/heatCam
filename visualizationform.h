#ifndef VISUALIZATIONFORM_H
#define VISUALIZATIONFORM_H

#include <QWidget>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QRegExp>

namespace Ui {
class VisualizationForm;
}

class VisualizationForm : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizationForm(QWidget *parent = 0);
    ~VisualizationForm();

signals:
    void signalXlsToCsv(QFile &file);

private slots:
    void on_loadPushButton_clicked();
    void slotXlsToCsv(QFile &file);

private:
    Ui::VisualizationForm *ui;
    QMap<int, QStringList> heatMatrixMap;
};

#endif // VISUALIZATIONFORM_H
