#ifndef SERIAL_H
#define SERIAL_H

#include <qglobal.h>
#define  BUFSIZE   1024
typedef union
{
   qint16 a[1024/2];
   char b[1024];
}serial_t;

typedef struct
{
    serial_t buf1;
    serial_t buf2;
    qint16 num1; //记录buf数量
    qint16 num2;
}buf_t;
#endif // SERIAL_H
