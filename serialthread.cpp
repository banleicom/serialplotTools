#include "serialthread.h"
#include <qDebug>
serialThread::serialThread()
{
   MainSerial = new QSerialPort();
   isStopped=true;
}

serialThread::~serialThread()
{
    delete MainSerial;
}

void serialThread::stop()
{
   isStopped=false;
}

void serialThread::run()
{
    qDebug("serialThread !");

    while(isStopped)
    {
        //SerialBuf.append(MainSerial->readAll());
        while(MainSerial->waitForReadyRead(1000)){
            SerialBuf.append(MainSerial->readAll());
            if(SerialBuf.size()>=BUFSIZE)
            {
                emit bufFull();
                qDebug()<<"in buf size"<<SerialBuf.size();
            }

            if(!isStopped)
                return;
        }
    }
}
