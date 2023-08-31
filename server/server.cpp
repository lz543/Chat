#include "server.h"
#include "ui_server.h"
#include <QRandomGenerator>
#include<QMessageBox>
#include<QDebug>
Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    this->imagesFolder=QDir::toNativeSeparators(QDir::currentPath() + QDir::separator() + "images");
    createImagesFolder();
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
    db=new DatabaseManager("127.0.0.1",3306,"2","root","lz2sx@1314");
    if(!db->connect()){
        qDebug()<<"数据库连接失败";
    }
    ui->label_6->setText(QString::number(number));


}

Server::~Server()
{
    if (tcpServer) {
        tcpServer->close();
        delete tcpServer;
        tcpServer = nullptr;
    }
    if(db != nullptr)
    {
        db->disconnect();  // 关闭数据库连接
        delete db;
        db = nullptr;
    }
    delete ui;
    ui=nullptr;
    clientMap.clear();
    socketToClientIdMap.clear();
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
        clientMap.clear();
        //offLineMessages.clear();
        ui->label_2->setText("已停止监听");
    }
}


// 在服务器端处理新的客户端连接
void Server::onNewConnection()
{
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket,&QTcpSocket::readyRead,this,&Server::onReadyRead);
}

//服务端收到消息
void Server::onReadyRead()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    QByteArray jsonData = clientSocket->readAll();
    processData(clientSocket,jsonData);
}

void Server::processData(QTcpSocket*clientSocket, const QByteArray& jsonData){
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject message = jsonDoc.object();

    // 提取消息类型
    QString messageType = message["type"].toString();

    // 根据消息类型进行不同的处理
    if (messageType == "login"){
        int userId = message["userId"].toInt();
        // 处理接收到的消息,当已经登录时发送No给客户端
        if(clientMap.contains(userId)) {
            // 向客户端发送失败响应
            QJsonObject response;
            response["type"] = "loginResponse";
            response["success"] = false;
            response["reason"]="你已经登录了，不能重复登录";
            QJsonDocument jsonResponse(response);
            clientSocket->write(jsonResponse.toJson());
            clientSocket->flush();

        }
        //未登录时，将登录ID对应tcpsocket存在QMap
        else{
            QString password = message["password"].toString();
            QSqlQuery query;
            // 准备查询语句
            query.prepare("SELECT name,password,image FROM user WHERE id = :str");
            query.bindValue(":str", userId);
            QJsonObject response;
            response["type"] = "loginResponse";
            if (query.exec()&&query.next()){
                QString passw = query.value("password").toString();
                if (passw==password) {
                    QString uname=query.value("name").toString();
                    QString imagePath=query.value("image").toString();
                    response["success"] = true;
                    response["name"]=uname;
                    QPixmap pixmap(imagePath);
                    QByteArray imageData;
                    QBuffer buffer(&imageData);
                    buffer.open(QIODevice::WriteOnly);
                    pixmap.save(&buffer, "jpg",50);
                    buffer.close();
                    response["imageData"] = QString(imageData.toBase64());
                    number++; //在线人数加一
                    ui->label_6->setText(QString::number(number));
                    clientMap[userId] = clientSocket;
                    socketToClientIdMap[clientSocket]=userId;
                    //断开连接时删除对应的Map的键值
                    connect(clientSocket, &QTcpSocket::disconnected, this, [=]() {
                        // 断开连接时删除对应的Map中的键值
                        if(socketToClientIdMap.contains(clientSocket)){
                            int clientIdToRemove = socketToClientIdMap.value(clientSocket);
                            socketToClientIdMap.remove(clientSocket);
                            clientMap.remove(clientIdToRemove);
                            this->number--;
                            ui->label_6->setText(QString::number(number));
                        }
                    });
                    //判断是否有离线消息
                    if(offLineMessages.contains(userId)){
                        response["offlineMessages"]=true;

                    }
                    else{
                        response["offlineMessages"]=false;
                    }
                    QJsonDocument jsonResponse(response);
                    clientSocket->write(jsonResponse.toJson());
                    clientSocket->flush();

                }
                else{

                    response["success"] = false;
                    response["reason"]="你输入的账号或密码有误";
                    QJsonDocument jsonResponse(response);
                    clientSocket->write(jsonResponse.toJson());
                    clientSocket->flush();
                }
            }
            else{

                response["success"] = false;
                response["reason"]="你输入的账号或密码有误";
                QJsonDocument jsonResponse(response);
                clientSocket->write(jsonResponse.toJson());
                clientSocket->flush();
            }

        }
    }
    else if (messageType == "message"){
        // 判断接收者是否在线并发送消息
        int receiverId = message["receiver_id"].toInt();
        QTcpSocket* receiverSocket = clientMap.value(receiverId,nullptr);
        if (receiverSocket) {
            // 发送消息
            receiverSocket->write(jsonData);
            receiverSocket->flush();//立即将待发送的数据从缓冲区刷新到套接字中
        } else {
            //不在线保存离线消息
            if (offLineMessages.contains(receiverId)) {
                offLineMessages[receiverId].enqueue(message);
            } else {
                QQueue<QJsonObject> messageQueue;
                messageQueue.enqueue(message);
                offLineMessages.insert(receiverId, messageQueue);
            }

        }
    }else if (messageType == "enroll"){
        qDebug()<<"收到l";
        QString name=message["enrollName"].toString();
        QString password=message["enrollPassword"].toString();

        QJsonObject response;
        response["type"] = "enrollResponse";
        bool isFlag=1;
        while(isFlag){
            int randomNumber = QRandomGenerator::global()->bounded(10000000, 1000000000);
            QString enrollId=QString::number(randomNumber);
            QString filePath = this->imagesFolder+QDir::separator()+enrollId+".jpg";//头像路径
            QSqlQuery query;
            // 准备查询语句
            qDebug()<<"准备插入";
            query.prepare("INSERT INTO user (id,password,name,image) VALUES (:id,:password,:name,:imagePath)");
            query.bindValue(":id",enrollId);
            query.bindValue(":password",password);
            query.bindValue(":name",name);
            query.bindValue(":imagePath",filePath);
            if(query.exec()){
                qDebug()<<"创建账号成功";
                response["success"]=true;
                response["id"]=enrollId;

                QByteArray imageData = QByteArray::fromBase64(message["imageData"].toString().toUtf8());
                QPixmap pixmap;
                pixmap.loadFromData(imageData, "jpg");

                // 保存图像数据到文件
                QFile imageFile(filePath);
                if (imageFile.open(QIODevice::WriteOnly)) {
                    imageFile.write(imageData);
                    imageFile.close();
                }


                isFlag=0;

            }else{
                qDebug() << "Error: ";
                //插入失败，账号已存在
                response["success"]=false;
            }
        }
        qDebug()<<"准备发送";
        QJsonDocument jsonResponse(response);
        clientSocket->write(jsonResponse.toJson());
        clientSocket->flush();
    }
    else if (messageType == "friends"){
        int userId=message["id"].toInt();
        QSqlQuery query;
        // 准备查询语句
        query.prepare("SELECT CASE WHEN id1=:id THEN id2 WHEN id2=:id THEN id1 END AS f_id FROM friendship WHERE (id1=:id OR id2=:id) and isFriend=1");
        query.bindValue(":id", userId);
        if (query.exec()) {
            QJsonArray friendsArray; // 用于存储好友信息
            while (query.next()) {
                int friendId = query.value(0).toInt();
                QSqlQuery query_2;
                query_2.prepare("SELECT name,image FROM user WHERE id=:f_id");
                query_2.bindValue(":f_id", friendId);
                if (query_2.exec() && query_2.next()) {
                    QString f_name = query_2.value("name").toString();
                    QString f_imagePath=query_2.value("image").toString();
                    QJsonObject friendInfo;
                    friendInfo["friendId"] =friendId;
                    friendInfo["friendName"] = f_name;
                    QPixmap pixmap(f_imagePath);
                    QByteArray imageData;
                    QBuffer buffer(&imageData);
                    buffer.open(QIODevice::WriteOnly);
                    pixmap.save(&buffer, "jpg",50);
                    buffer.close(); // 关闭缓冲区以刷新数据
                    friendInfo["imageData"] = QString(imageData.toBase64());
                    friendsArray.append(friendInfo);
                }
            }
            QJsonObject response;
            response["type"] = "friendsResponse";
            response["friends"] = friendsArray;
            QJsonDocument jsonResponse(response);

            QByteArray jsonData = jsonResponse.toJson();
            qint64 totalSize = jsonData.size();
            clientSocket->write(QByteArray::number(totalSize));
            clientSocket->waitForBytesWritten();
            QThread::msleep(30);
            // 分片发送消息

            int chunkSize = 1024; // 每个片段的大小

            int sentBytes = 0;
            while (sentBytes < totalSize) {
                int remainingBytes = totalSize - sentBytes;
                int bytesToSend = qMin(chunkSize, remainingBytes);
                // 发送当前片段消息
                QByteArray chunkData = jsonData.mid(sentBytes, bytesToSend);
                clientSocket->write(chunkData);
                clientSocket->waitForBytesWritten();
                sentBytes += bytesToSend;
            }
        }
    }else if (messageType == "search"){
        int searchId = message["searchId"].toInt();
        QJsonObject response;
        response["type"] = "searchResponse";
        QSqlQuery query;
        // 准备查询语句
        query.prepare("SELECT name FROM user where id=:id");
        query.bindValue(":id",searchId);
        if(query.exec()&&query.next()){
            //查找成功且有结果
            response["success"]=true;
            response["searchName"]=query.value(0).toString();
            response["searchId"]=searchId;
        }else{
            response["success"]=false;
        }
        QJsonDocument jsonResponse(response);
        clientSocket->write(jsonResponse.toJson());
        clientSocket->flush();
        clientSocket->waitForBytesWritten();
    }else if (messageType == "add"){
        int addId=message["addId"].toInt();
        int id=message["id"].toInt();
        QString time=message["time"].toString();
        QString remark=message["remark"].toString();
        QJsonObject response;
        response["type"]="add";
        response["id"]=id;
        response["time"]=time;
        response["remark"]=remark;
        QSqlQuery query;
        query.prepare("SELECT name,image FROM user WHERE id=:id");
        query.bindValue(":id", id);
        if (query.exec() && query.next()) {
            QString name = query.value("name").toString();
            QString imagePath=query.value("image").toString();
            QPixmap pixmap(imagePath);
            QByteArray imageData;
            QBuffer buffer(&imageData);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "jpg",50);
            buffer.close(); // 关闭缓冲区以刷新数据
            response["name"]=name;
            response["imageData"] = QString(imageData.toBase64());
        }
        QJsonDocument jsonResponse(response);
        QTcpSocket* receiverSocket = clientMap.value(addId);
        if (receiverSocket) {
            // 发送消息
            receiverSocket->write(jsonResponse.toJson());
            receiverSocket->flush();//立即将待发送的数据从缓冲区刷新到套接字
        } else {
            //不在线保存离线消息
            offLineMessages[addId].enqueue(message);
        }

    } else if(messageType == "Rmessage"){
        int userId = message["userId"].toInt();
        QQueue<QJsonObject>& messageQueue = offLineMessages[userId];
        while (!messageQueue.isEmpty()) {
            QJsonObject offlineMessage = messageQueue.dequeue();
            QJsonDocument jsonDoc_2(offlineMessage);
            clientSocket->write(jsonDoc_2.toJson());
            clientSocket->flush(); // 立即将待发送的数据从缓冲区刷新到套接字中
            clientSocket->waitForBytesWritten();
        }
    }else if(messageType == "addResponse"){
        int id=socketToClientIdMap.value(clientSocket);
        int sendId=message["sendId"].toInt();
        int success=message["accept?"].toInt();
        QString time=message["time"].toString();
        if(success==1){
            QSqlQuery query;
            // 准备插入语句
            query.prepare("INSERT INTO friendship (id1,id2,isFriend) VALUES (:sendId,:id,1)");
            query.bindValue(":sendId", sendId);
            query.bindValue(":id", id);
            if(query.exec()){
                qDebug()<<"好友添加成功";
            }
            QJsonObject response;
            response["type"] = "addResponse";
            response["addId"] = id;
            response["time"] = time;
            response["success"] = success;
            QSqlQuery query_2;
            query_2.prepare("SELECT name,image FROM user WHERE id=:id");
            query_2.bindValue(":id", id);
            if (query_2.exec() && query_2.next()){
                QString name = query_2.value("name").toString();
                QString imagePath=query_2.value("image").toString();
                QPixmap pixmap(imagePath);
                QByteArray imageData;
                QBuffer buffer(&imageData);
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer, "jpg",50);
                buffer.close(); // 关闭缓冲区以刷新数据
                response["addName"]=name;
                response["imageData"] = QString(imageData.toBase64());
            }
            QTcpSocket* receiverSocket = clientMap.value(sendId);
            if (receiverSocket) {
                // 发送消息
                QJsonDocument jsonResponse(response);
                receiverSocket->write(jsonResponse.toJson());
                receiverSocket->flush();//立即将待发送的数据从缓冲区刷新到套接字
            } else {
                //不在线保存离线消息
                offLineMessages[sendId].enqueue(message);
            }
        }    
    }
}


