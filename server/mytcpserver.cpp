
#include "mytcpserver.h"
#include "clientHandler.h"
TcpServer::TcpServer(QTcpServer *parent) : QTcpServer(parent)
{

}

TcpServer::~TcpServer()
{

}

void TcpServer::start(const QString &ip,int port){
    QHostAddress addr;
    addr.setAddress(ip);

    bool success = this->listen(addr, port);
    if (success) {
        qDebug()<<"开始监听";
    } else {
        qDebug()<<"监听失败";
    }

}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    QThread* thread = new QThread;
    ClientHandler* clientHandler = new ClientHandler(nullptr, socketDescriptor);
    clientHandler->moveToThread(thread);

    connect(thread, &QThread::started, clientHandler, &ClientHandler::run);
    connect(clientHandler, &ClientHandler::finish, thread, &QThread::quit);
    connect(clientHandler, &ClientHandler::finish, clientHandler, &ClientHandler::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();

    /*ClientHandler * c=new ClientHandler(socketDescriptor);
    threadPool.globalInstance()->start(c);




    ClientHandler *c = new ClientHandler(this, socketDescriptor);
    m_threadPool.start(c);*/


    connect(clientHandler,&ClientHandler::ok,this,[=](){
            emit newConnection();
        }, Qt::QueuedConnection);  //文档要求继承本函数需要发射此信号
}

