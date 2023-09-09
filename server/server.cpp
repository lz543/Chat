

#include "server.h"
#include "ui_server.h"
#include "clientHandler.h"
#include <QRandomGenerator>
#include<QMessageBox>
#include<QDebug>
Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    //头像路径
    this->imagesFolder=QDir::toNativeSeparators(QDir::currentPath() + QDir::separator() + "images");
    //创建文件夹
    createImagesFolder();
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
    ui->label_6->setText(QString::number(number));
    threadPool.setMaxThreadCount(8);


}

Server::~Server()
{
    if (tcpServer) {
        tcpServer->close();
        delete tcpServer;
        tcpServer = nullptr;
    }
    delete ui;
    ui=nullptr;
    /*clientMap.clear();
    socketToClientIdMap.clear();*/
}

void Server::createImagesFolder()
{
    if (!QDir(imagesFolder).exists()) {
        QDir().mkdir(imagesFolder);
    }
}

//开始监听
void Server::on_pushButton_clicked()
{
    QString address = ui->lineEdit->text();
    int port = ui->lineEdit_2->text().toInt();
    QHostAddress addr;
    if (addr.setAddress(address)&& addr.protocol() == QAbstractSocket::IPv4Protocol) {
        if (!tcpServer->isListening()) {
            bool success = tcpServer->listen(addr, port);
            if (success) {
                ui->label_2->setText("正在监听");
            } else {
                ui->label_2->setText("监听失败");
            }
        }
    } else {
        ui->label_2->setText("ip输入有误");
    }
}


//停止监听
void Server::on_pushButton_2_clicked()
{
    tcpServer->close(); // 停止监听
    if (tcpServer->isListening()) {
        ui->label_2->setText("停止监听失败");
    } else {
        number=0;
        ui->label_6->setText(QString::number(number));
        //clientMap.clear();
        //offLineMessages.clear();
        ui->label_2->setText("已停止监听");
    }
}


// 在服务器端处理新的客户端连接
void Server::onNewConnection()
{
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();//获取连接的客户端的socket
    connect(clientSocket,&QTcpSocket::readyRead,this,[=](){
        QByteArray jsonData = clientSocket->readAll();
        // 创建一个 ClientHandler 对象
        ClientHandler *clientHandler=new ClientHandler(clientSocket,jsonData);
        threadPool.start(clientHandler);
    });
}

//服务端收到消息
/*void Server::onReadyRead()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    QByteArray jsonData = clientSocket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject message = jsonDoc.object();

    // 提取消息类型
    QString messageType = message["type"].toString();

    // 根据消息类型进行不同的处理
    if (messageType == "login"){
        processLogin(clientSocket, message);
    }
    else if (messageType == "message"){
        // 判断接收者是否在线并发送消息
        processMessage(message);
    }else if (messageType == "enroll"){
        processEnroll(clientSocket, message);
    }
    else if (messageType == "friends"){
        processFriends(clientSocket, message);
    }else if (messageType == "search"){
        processSearch(clientSocket, message);
    }else if (messageType == "add"){
        processAdd(message);
    } else if(messageType == "Rmessage"){
        processRmessage(clientSocket, message);
    }else if(messageType == "addResponse"){
        processAddResponse(clientSocket, message);
    }
}*/


