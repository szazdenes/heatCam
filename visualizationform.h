#ifndef VISUALIZATIONFORM_H
#define VISUALIZATIONFORM_H

#include <QWidget>
#include <QFileDialog>
#include <QTextStream>
#include <QProcess>
#include <QRegExp>
#include <QGraphicsScene>
#include <QPainter>
#include <QToolTip>
#include <QDebug>

#include "temperaturedialog.h"
#include "heatmapdialog.h"

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
    void signalSendTminTmax(double min, double max);
    void signalSendPalette(int palette);
    void signalSendLineData(QStringList lineData);
    void signalSendAreaData(QStringList areaData, QString filename);
    void signalLineOff();
    void signalLineOn();
    void signalSendFilename(QString fileName);
    void signalSendHeatImage(QString heatImageName);

public slots:
    void on_savePushButton_clicked();

private slots:
    void on_loadPushButton_clicked();
    void slotXlsToCsv(QFile &file);

    void on_fiPushButton_clicked();

    void on_origialPushButton_clicked();

    void on_tempPushButton_clicked();
    void slotTempMinMax(double min, double max);
    void slotPalletteChanged(int palette);

    void on_clearPushButton_clicked();
    void slotMouseMoved(QPointF pos);
    void slotMouseButtonPressed(QPointF pos);
    void slotMouseButtonReleased(QPointF pos);
    void slotWheelUp();
    void slotWheelDown();
    void getDataFromHeatLine();

    void on_clearMaskPushButton_clicked();

    void on_lineCheckBox_toggled(bool checked);

    void on_areaCheckBox_toggled(bool checked);

    void on_areaToTablePushButton_clicked();

    void on_openHeatImagePushButton_clicked();

private:
    Ui::VisualizationForm *ui;

    void drawHeatMap(QMap<int, QStringList> &heatMap);
    QColor getPixelColor(int palette, double temp, double tMin, double tMax);
    void refreshImageMask();
    void refreshHeatlineMask(QPoint startPos, QPoint endPos);
    void setCursorImage(double size);
    void refreshMask();
    void paintMask(QPointF pos);


    QMap<int, QStringList> heatMatrixMap;
    QString heatFilenamecsv;
    QString openFileName;
    QString openImageName;
    QImage image;
    QGraphicsScene scene;
    double zoom;
    double tmin, tmax;
    int paletteNum;
    void rapidEvaluation();
    bool imageLoaded;
    bool isButtonPressed, isButtonReleased;
    QPoint start, end;
    QImage mask;
    QImage mainImage;
    QImage maskedImage;
    bool isHeatLineOn;
    QCursor cursor;
    double pensize;
};

#endif // VISUALIZATIONFORM_H
