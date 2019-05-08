#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include "serialthread.h"

#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_directpainter.h"
#include "qwt_plot_zoomer.h"
#include "qwt_color_map.h"
#include "qwt_plot_spectrogram.h"
#include "qwt_scale_widget.h"
#include "qwt_scale_draw.h"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_renderer.h"
#include "qwt_plot_canvas.h"
#include "qwt_legend.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();
private:
    serialThread *serial;
    qint16 MainSerialPortRecvFrameNumber;
    qint16 MainSerialPortRecvErrorFrameNumber;
    const qint16 MainSerialPortOneFrameSize=1*1024*1024;

    QFile *file;
private slots:
    void MainSerialRecvMsgEvent();
    void serialfull();

    void on_okButton_toggled(bool checked);
    void on_clearButton_toggled(bool checked);
    void on_pushButton_clicked();
    void on_checkBox_word_stateChanged(int arg1);
    void on_checkBox_hex_stateChanged(int arg1);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QwtPlotCurve *d_curve1;
    QwtPlotZoomer *zoomer;
    QwtPlotPanner *panner;

    QVector<double> valuesx;
    QVector<double> valuesy;

    struct timeval tpstart,tpend;

    bool wordOrder; //字序 1小端  0大端
    bool bin;       //二进制显示 1二进制  0ASCLL

};

#endif // MAINWINDOW_H
