#ifndef SERVER_TCP_H
#define SERVER_TCP_H

#include <QTcpSocket>
#include <QTcpServer>


class server_TCP
{

private:
    QTcpServer *server;
    QTcpSocket socket;

public:
    server_TCP();
};

#endif // SERVER_TCP_H
