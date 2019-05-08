#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H
#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
/*大小端转换*/
#define BIG2LITTLE16(A)   A=((((A) & 0xFF00) >> 8) | (((A) & 0x00FF) << 8))
#define BIG2LITTLE32(A)   A=((((uint32_t)(A) & 0xff000000) >> 24) | \
                           (((uint32_t)(A) & 0x00ff0000) >> 8)  | \
                           (((uint32_t)(A) & 0x0000ff00) << 8)  | \
                           (((uint32_t)(A) & 0x000000ff) << 24))

#define BUFSIZE  300
class serialThread : public QThread
{
    Q_OBJECT
public:
    serialThread();
    ~serialThread();

    QSerialPort *MainSerial;
    QByteArray SerialBuf;
    void stop();

protected:
    void run();
    bool isStopped;
    bool buff;
Q_SIGNALS:
    void bufFull();
};

#endif // SERIALTHREAD_H
