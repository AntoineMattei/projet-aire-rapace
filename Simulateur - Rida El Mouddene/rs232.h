#ifndef RS232_H
#define RS232_H
#include "mainwindow.h"
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <QString>

class RS232
{
public:
    RS232();

    int OpenSerial();
    int SendSerial(int, QByteArray);
    void close_fd(int);

    int OpenSerial2();
    int SendSerial2(int, QByteArray);
    void close_fd2(int);
};

#endif // RS232_H
