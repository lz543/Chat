
//TcpSocket.cpp
#include "mytcpsocket.h"

TcpSocket::TcpSocket(int sock, QTcpSocket *parent) : QTcpSocket(parent)
{
    this->setSocketDescriptor(sock);
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(SocketErr(QAbstractSocket::SocketError)));
}

TcpSocket::~TcpSocket()
{

}

// 将会移进线程里面运行
void TcpSocket::ReadAndParseData()
{
    TcpSocket *socket = (TcpSocket*)sender();
    QString recvStr = socket->readAll();
    m_recvDataStr += recvStr;
    qDebug()<<m_recvDataStr<<socket->localAddress().toString();
    // 对数据的处理
}

void TcpSocket::SocketErr(QAbstractSocket::SocketError socketError)
{
    TcpSocket *socket = (TcpSocket*)sender();
}






