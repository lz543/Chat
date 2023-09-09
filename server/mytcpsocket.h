//TcpSocket.h
#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QHostAddress>

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(int socketdesc,QTcpSocket *parent = NULL);
    ~TcpSocket();

private slots:
    void ReadAndParseData();
    void SocketErr(QAbstractSocket::SocketError socketError);
private:
    QString m_recvDataStr;
};

#endif // TCPSOCKET_H

