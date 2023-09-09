
#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H


#include <QRunnable>
#include <QDebug>
#include<QtNetwork>
#include <QTimer>
#include <QThread>
#include "connectionPool.h"

static QMap<int, QTcpSocket*> clientMap;
static QMap<QTcpSocket*, int> socketToClientIdMap;
static QMap<int, QQueue<QJsonObject>> offLineMessages;

class ClientHandler : public QObject
{
    Q_OBJECT
public:
    ClientHandler (QObject * parent,qintptr socketDescriptor) : QObject(parent),socketDescriptor(socketDescriptor)
    {
        //db = ConnectionPool::openConnection();
    }

    void run()
    {
        QTcpSocket *clientSocket=new QTcpSocket(this);
        if (!clientSocket->setSocketDescriptor(socketDescriptor)) {
            //qDebug() << "连接失败";
            clientSocket->deleteLater();
            emit finish();
            return;
        }
        db = ConnectionPool::openConnection();
        qDebug() << "Client connected on thread" << QThread::currentThread();
        // 在这里执行消息处理的具体逻辑
        connect(clientSocket,&QTcpSocket::readyRead,this,[=](){
            QByteArray jsonData = clientSocket->readAll();
            // 创建一个 ClientHandler 对象
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
        });
        emit ok();
    }

private:
    QMutex mutex;//互斥锁
    qintptr socketDescriptor;
    QSqlDatabase db;//数据库连接
    QString imagesFolder=QDir::toNativeSeparators(QDir::currentPath() + QDir::separator() + "images");
    QByteArray imageToByteArray(const QString &imagePath,int quality=50);
    void processMessage(const QJsonObject& message);
    void processAdd(const QJsonObject& message);
    void processLogin(QTcpSocket* clientSocket, const QJsonObject& message);
    void processEnroll(QTcpSocket* clientSocket, const QJsonObject& message);
    void processFriends(QTcpSocket* clientSocket, const QJsonObject& message);
    void processSearch(QTcpSocket* clientSocket, const QJsonObject& message);
    void processRmessage(QTcpSocket* clientSocket, const QJsonObject& message);
    void processAddResponse(QTcpSocket* clientSocket, const QJsonObject& message);

signals:
    void finish();
    void ok();

};

#endif // CLIENTHANDLER_H
