#include "rs232.h"

RS232::RS232()
{
}

int RS232::OpenSerial()
{
    struct termios options;

    int fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NDELAY);

    fcntl(fd, F_SETFL, 0);
    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~PARENB;

    tcsetattr(fd, TCSANOW, &options);

    return(fd);
}

int RS232::SendSerial(int fd,QByteArray trame)
{
    if( write(fd,trame,strlen(trame)) == -1 )
    {
        return(-1);
    }
    return(1);
}

void RS232::close_fd(int fd)
{
    close(fd);
}


int RS232::OpenSerial2()
{
    struct termios options;

    int fd = open("/dev/ttyUSB2", O_RDWR | O_NOCTTY | O_NDELAY);

    fcntl(fd, F_SETFL, 0);
    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~PARENB;

    tcsetattr(fd, TCSANOW, &options);

    return(fd);
}

int RS232::SendSerial2(int fd, QByteArray trame)
{
    if( write(fd,trame,strlen(trame)) == -1 )
    {
        return(-1);
    }
    return(1);

}

void RS232::close_fd2(int fd)
{
    close(fd);
}
