
#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H


#include <QThread>
#include <QtNetwork>
#include <QList>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(QTcpServer *parent = NULL);
    ~TcpServer();
    void start(const QString &ip,int port);

protected:
    void incomingConnection(qintptr socketDescriptor);


};
#endif // MYTCPSERVER_H
