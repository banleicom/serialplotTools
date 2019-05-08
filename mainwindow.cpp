#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtWidgets>
#include <QMessageBox>
#include <sys/time.h>

/*
=======================================
    Zoomer 类
=======================================
*/
class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer(QWidget *canvas):
        QwtPlotZoomer(canvas)
    {
        setTrackerMode(ActiveOnly);
    }

    virtual QwtText trackerTextF(const QPointF &pos) const
    {
        QColor bg(Qt::white);
        bg.setAlpha(200);

        QwtText text = QwtPlotZoomer::trackerTextF(pos);
        text.setBackgroundBrush(QBrush(bg));
        return text;
    }
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboBox_2->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->comboBox_2->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->comboBox_2->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->comboBox_2->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    ui->comboBox_2->setCurrentIndex(3);
    ui->okButton->setCheckable(true);
    ui->clearButton->setCheckable(true);
    ui->okButton->setEnabled(false);

    /* 设置坐标 */
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom, 0, 10*1024);
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft, -6096, +6096);
    ui->qwtPlot->setAxisFont(QwtPlot::xBottom, QFont("Times", 8, QFont::Normal));
    ui->qwtPlot->setAxisFont(QwtPlot::yLeft, QFont("Times", 8, QFont::Normal));

    /* 设置画布 */
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setFrameStyle(QFrame::Box|QFrame::Plain);
    canvas->setLineWidth(1);
    canvas->setPalette(Qt::white);
    ui->qwtPlot->setCanvas(canvas);

    /* 设置网格 */
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(Qt::gray, 0.0, Qt::DotLine);
    grid->attach(ui->qwtPlot);

    /* 添加曲线 */
    d_curve1 = new QwtPlotCurve();
    d_curve1->setPen(Qt::blue, 1);
    d_curve1->setRenderHint(QwtPlotItem::RenderAntialiased, false);  /* 禁用抗锯齿 */
    //d_curve1->setPaintAttribute(QwtPlotCurve::FilterPointsAggressive, true);  /* 使能滤波算法 */
    d_curve1->setLegendAttribute( QwtPlotCurve::LegendNoAttribute, true);
    d_curve1->attach(ui->qwtPlot);

    /* 默认左键选中放大，右键恢复上级，CTRL+右键恢复原样 */
    zoomer = new MyZoomer(ui->qwtPlot->canvas());
    zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

    /* 左键平移 */
    panner = new QwtPlotPanner(ui->qwtPlot->canvas());
    panner->setEnabled(false);

    /*容器初始化*/
    //valuesx.resize(360*2000);
    for(int i=0;i<10*1024;i++)
    {
        valuesx<<(double)i;
    }
    valuesy.resize(10*1024);

    qDebug()<<"valusex/y size"<<valuesx.size()<<valuesy.size();

    /*文件初始化*/
    file=new QFile();
    serial=NULL;

    /*按钮初始化*/
    if(ui->checkBox_word->isChecked()){
        wordOrder=1;
    }else{
        wordOrder=0;
    }
    if(ui->checkBox_hex->isChecked()){
        bin=1;
    }else{
        bin=0;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 串口接收数据事件回调函数 */
void MainWindow::MainSerialRecvMsgEvent()
{
    //    MainSerialRecvData.append(QString(serial->MainSerial->readAll()));

    //    if(MainSerialRecvData.size()>1000)
    //    {
    //       ui->textEdit->append(QString(MainSerialRecvData));
    //       MainSerialRecvData.clear();
    //        //file->write(buf);
    //    }

}

void MainWindow::on_okButton_toggled(bool checked)
{
    /*打开串口*/
    if (serial==NULL)
    {
        /*创建线程*/
        serial=new serialThread();
        connect(serial, SIGNAL(bufFull()), this, SLOT(serialfull()), Qt::QueuedConnection);
        /* 判断是否有可用串口 */
        if(ui->comboBox_2->count() != 0)
        {
            /* 串口已经关闭，现在来打开串口 */
            /* 设置串口名称 */
            serial->MainSerial->setPortName(ui->comboBox_1->currentText());
            /* 设置波特率 */
            serial->MainSerial->setBaudRate(QString(ui->comboBox_2->currentText()).toInt());
            /* 设置数据位数 */
            serial->MainSerial->setDataBits(QSerialPort::Data8);
            /* 设置奇偶校验 */
            serial->MainSerial->setParity(QSerialPort::NoParity);
            /* 设置停止位 */
            serial->MainSerial->setStopBits(QSerialPort::OneStop);
            /* 设置流控制 */
            serial->MainSerial->setFlowControl(QSerialPort::NoFlowControl);
            /* 打开串口 */
            serial->MainSerial->open(QIODevice::ReadWrite);
            /* 设置串口缓冲区大小，这里必须设置为这么大 */
            serial->MainSerial->setReadBufferSize(MainSerialPortOneFrameSize);
            /* 注册回调函数 */
            //QObject::connect(MainSerial, &QSerialPort::readyRead, this, &MainWindow::MainSerialRecvMsgEvent);

            /*UI*/
            ui->okButton->setText(tr("关闭串口"));
            ui->okButton->setStyleSheet(QString("QPushButton{color:red}"));
            ui->clearButton->setEnabled(false);

            serial->start();

            file->setFileName("read.dat");
            if(!file->open(QIODevice::WriteOnly))
            {
                qDebug()<<"文件打开失败";
            }
            qDebug()<<"串口在关闭状态，现在打开了" + ui->comboBox_1->currentText();

        }else{
            qDebug()<<"没有可用串口，请重新常识扫描串口";
            // 警告对话框
            QMessageBox::warning(this,tr("警告"),tr("没有可用串口，请重新尝试扫描串口！"),QMessageBox::Ok);
        }

    }else{
        /* 关闭串口 */
        serial->MainSerial->close();
        ui->okButton->setText(tr("打开串口"));
        ui->okButton->setStyleSheet(QString("QPushButton{color:black}"));
        ui->clearButton->setEnabled(true);

        serial->stop();
        //serial->exit();
        while(!serial->isFinished()) {;}
        disconnect(serial, SIGNAL(bufFull()), this, SLOT(serialfull()));
        delete  serial;
        serial=NULL;

        file->close();
        qDebug()<<"串口在打开状态，串口关闭";
    }
}

void MainWindow::on_clearButton_toggled(bool checked)
{
    /* 查找可用串口 */
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        /* 判断端口是否能打开 */
        if(serial.open(QIODevice::ReadWrite))
        {
            int isHaveItemInList = 0;
            /* 判断是不是已经在列表中了 */
            for(int i=0; i<ui->comboBox_2->count(); i++)
            {
                /* 如果，已经在列表中了，那么不添加这一项了就 */
                if(ui->comboBox_1->itemText(i) == serial.portName())
                {
                    isHaveItemInList++;
                }
            }

            if(isHaveItemInList == 0)
            {
                ui->comboBox_1->addItem(serial.portName());
            }
            serial.close();
        }
    }
    ui->okButton->setEnabled(true);
}

void MainWindow::serialfull()
{
    //gettimeofday(&tpstart,NULL);//debug
    union type_t
    {
        qint8 buf8[BUFSIZE];
        qint16 buf16[BUFSIZE/2];
    }buf;

    /*更新ui*/
    if(bin){
        ui->textEdit->append(QString(serial->SerialBuf.toHex()));
    }else{

    }

    //qDebug()<<"int buf size"<<fifo.size();

    //if(fifo.size()<BUFSIZE*2) return;
    for(int i=0;i<BUFSIZE;i++)
    {
       buf.buf8[i]=serial->SerialBuf.at(i);
    }
    serial->SerialBuf.remove(0,BUFSIZE);
    //qDebug()<<"out buf size"<<serial->SerialBuf.size();
    valuesy.erase(valuesy.begin(),valuesy.begin()+BUFSIZE/2);
    /*大小端转换*/
    if(wordOrder == 0){
        for(int i=0;i<BUFSIZE/2;i++)
        {
            BIG2LITTLE16(buf.buf16[i]);
            //valuesy<<(double)buf.buf16[i];
        }
    }
    for(int i=0;i<BUFSIZE/2;i++)
    {
        valuesy<<(double)buf.buf16[i];
    }
    /*更新曲线数据*/
    d_curve1->setSamples(valuesx,valuesy);
    ui->qwtPlot->replot();

    //gettimeofday(&tpend,NULL);//debug
    //double timeuse=(1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000000.0;

    //qDebug()<<timeuse<<"s";
}

void MainWindow::on_pushButton_clicked()
{
    qDebug("clear!");
    ui->textEdit->clear();
}

void MainWindow::on_checkBox_word_stateChanged(int arg1)
{
    if(ui->checkBox_word->isChecked()){
        wordOrder=1;
    }else{
        wordOrder=0;
    }
}

void MainWindow::on_checkBox_hex_stateChanged(int arg1)
{
    if(ui->checkBox_hex->isChecked()){
        bin=1;
    }else{
        bin=0;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
   valuesy.clear();
   valuesy.resize(10*1024);
   d_curve1->setSamples(valuesx,valuesy);
   ui->qwtPlot->replot();
}
