#include "clientHandler.h"
#include<QSqlQuery>



//将图片路径转化为二进制数组
QByteArray ClientHandler::imageToByteArray(const QString &imagePath,int quality)
{
    QPixmap pixmap(imagePath);
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "jpg", quality);
    buffer.close();
    return imageData.toBase64();
}

//处理登录
void ClientHandler::processLogin(QTcpSocket* clientSocket, const QJsonObject& message){
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
        QSqlQuery query(db);
        // 准备查询语句
        query.prepare("SELECT name,password,image FROM user WHERE id = :str");
        query.bindValue(":str", userId);
        QJsonObject response;
        response["type"] = "loginResponse";
        if (query.exec()&&query.next()){
            QString passw = query.value("password").toString();
            if (passw==password) {
                response["success"] = true;
                response["name"]=query.value("name").toString();
                QString imagePath=query.value("image").toString();
                response["imageData"] = QString(imageToByteArray(imagePath));
                {
                    QMutexLocker locker(&mutex); // 加锁
                    clientMap[userId] = clientSocket;
                    socketToClientIdMap[clientSocket] = userId;
                } // 在作用域结束时自动解锁
                //断开连接时删除对应的Map的键值
                connect(clientSocket, &QTcpSocket::disconnected, this, [=]() {
                    // 断开连接时删除对应的Map中的键值
                    if(socketToClientIdMap.contains(clientSocket)){
                        int clientIdToRemove = socketToClientIdMap.value(clientSocket);                      
                        {
                            QMutexLocker locker(&mutex); // 加锁
                            socketToClientIdMap.remove(clientSocket);
                            clientMap.remove(clientIdToRemove);
                        }
                        emit finish();//断开连接时发送结束信号
                    }
                });
                //判断是否有离线消息
                if(offLineMessages.contains(userId)){
                    response["offlineMessages"]=true;
                }
                else{
                    response["offlineMessages"]=false;
                }
            }
            else{
                response["success"] = false;
                response["reason"]="你输入的账号或密码有误";
            }
        }
        else{
            response["success"] = false;
            response["reason"]="你输入的账号或密码有误";
        }
        QJsonDocument jsonResponse(response);
        clientSocket->write(jsonResponse.toJson());
        clientSocket->flush();
    }
}

//处理消息
void ClientHandler::processMessage( const QJsonObject& message){
    int receiverId = message["receiver_id"].toInt();
    QTcpSocket* receiverSocket = clientMap.value(receiverId,nullptr);
    if (receiverSocket) {
        // 发送消息
        QJsonDocument jsonResponse(message);
        receiverSocket->write(jsonResponse.toJson());
        receiverSocket->flush();//立即将待发送的数据从缓冲区刷新到套接字中
    } else {
        //不在线保存离线消息
        if (offLineMessages.contains(receiverId)) {
            offLineMessages[receiverId].enqueue(message);
        } else {
            QQueue<QJsonObject> messageQueue;
            messageQueue.enqueue(message);
            {
                QMutexLocker locker(&mutex); // 加锁
                offLineMessages.insert(receiverId, messageQueue);
            }
        }
    }
}

//处理注册
void ClientHandler::processEnroll(QTcpSocket* clientSocket, const QJsonObject& message){
    QString name=message["enrollName"].toString();
    QString password=message["enrollPassword"].toString();
    QJsonObject response;
    response["type"] = "enrollResponse";
    bool isFlag=1;
    while(isFlag){
        int randomNumber = QRandomGenerator::global()->bounded(10000000, 1000000000);
        QString enrollId=QString::number(randomNumber);
        QString filePath = this->imagesFolder+QDir::separator()+enrollId+".jpg";//头像路径
        QSqlQuery query(db);
        // 准备查询语句
        qDebug()<<"准备插入";
        query.prepare("INSERT INTO user (id,password,name,image) VALUES (:id,:password,:name,:imagePath)");
        query.bindValue(":id",enrollId);
        query.bindValue(":password",password);
        query.bindValue(":name",name);
        query.bindValue(":imagePath",filePath);
        {
            QMutexLocker locker(&mutex); // 加锁
            if(query.exec()){
                //qDebug()<<"创建账号成功";
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
            }
        }
    }
    QJsonDocument jsonResponse(response);
    clientSocket->write(jsonResponse.toJson());
    clientSocket->flush();
}

//处理好友列表
void ClientHandler::processFriends(QTcpSocket* clientSocket, const QJsonObject& message){
    int userId=message["id"].toInt();
    QSqlQuery query(db);
    // 准备查询语句
    query.prepare("SELECT CASE WHEN id1=:id THEN id2 WHEN id2=:id THEN id1 END AS f_id FROM friendship WHERE (id1=:id OR id2=:id) and isFriend=1");
    query.bindValue(":id", userId);
    if (query.exec()) {
        QJsonArray friendsArray; // 用于存储好友信息
        while (query.next()) {
            int friendId = query.value(0).toInt();
            QSqlQuery query_2(db);
            query_2.prepare("SELECT name,image FROM user WHERE id=:f_id");
            query_2.bindValue(":f_id", friendId);
            if (query_2.exec() && query_2.next()) {
                QJsonObject friendInfo;
                friendInfo["friendName"] = query_2.value("name").toString();
                QString f_imagePath=query_2.value("image").toString();
                friendInfo["friendId"] =friendId;
                friendInfo["imageData"] = QString(imageToByteArray(f_imagePath));
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
}

//处理查找
void ClientHandler::processSearch(QTcpSocket* clientSocket, const QJsonObject& message){
    int searchId = message["searchId"].toInt();
    QJsonObject response;
    response["type"] = "searchResponse";
    QSqlQuery query(db);
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
}

//处理添加
void ClientHandler::processAdd(const QJsonObject& message){
    int addId=message["addId"].toInt();
    int id=message["id"].toInt();
    QString time=message["time"].toString();
    QString remark=message["remark"].toString();
    QJsonObject response;
    response["type"]="add";
    response["id"]=id;
    response["time"]=time;
    response["remark"]=remark;
    QSqlQuery query(db);
    query.prepare("SELECT name,image FROM user WHERE id=:id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) {
        QString name = query.value("name").toString();
        QString imagePath=query.value("image").toString();
        response["name"]=name;
        response["imageData"] = QString(imageToByteArray(imagePath));
    }
    QJsonDocument jsonResponse(response);
    QTcpSocket* receiverSocket = clientMap.value(addId);
    if (receiverSocket) {
        // 发送消息
        receiverSocket->write(jsonResponse.toJson());
        receiverSocket->flush();//立即将待发送的数据从缓冲区刷新到套接字
    } else {
        //不在线保存离线消息
        {
            QMutexLocker locker(&mutex); // 加锁
            offLineMessages[addId].enqueue(message);
        }
    }
}

//处理获取不在线时的离线消息
void ClientHandler::processRmessage(QTcpSocket* clientSocket, const QJsonObject& message){
    int userId = message["userId"].toInt();
    QQueue<QJsonObject>& messageQueue = offLineMessages[userId];
    while (!messageQueue.isEmpty()) {
        QJsonObject offlineMessage = messageQueue.dequeue();
        QJsonDocument jsonDoc_2(offlineMessage);
        clientSocket->write(jsonDoc_2.toJson());
        clientSocket->flush(); // 立即将待发送的数据从缓冲区刷新到套接字中
        clientSocket->waitForBytesWritten();
    }
}

//处理客户端对添加的回应
void ClientHandler::processAddResponse(QTcpSocket* clientSocket, const QJsonObject& message){
    int id=socketToClientIdMap.value(clientSocket);
    int sendId=message["sendId"].toInt();
    int success=message["accept?"].toInt();
    QString time=message["time"].toString();
    if(success==1){
        QSqlQuery query(db);
        // 准备插入语句
        query.prepare("INSERT INTO friendship (id1,id2,isFriend) VALUES (:sendId,:id,1)");
        query.bindValue(":sendId", sendId);
        query.bindValue(":id", id);
        {
            QMutexLocker locker(&mutex); // 加锁
            if(query.exec()){
                qDebug()<<"好友添加成功";
            }
        }
        QJsonObject response;
        response["type"] = "addResponse";
        response["addId"] = id;
        response["time"] = time;
        response["success"] = success;
        QSqlQuery query_2(db);
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
            {
                QMutexLocker locker(&mutex); // 加锁
                offLineMessages[sendId].enqueue(message);
            }
        }
    }
}
