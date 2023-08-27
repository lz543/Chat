#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include<QtNetwork>
#include"databasemanager.h"
namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    int number=0;
    explicit Server(QWidget *parent = nullptr);
    ~Server();


private slots:
    void on_pushButton_clicked();
    void onReadyRead();
    void onNewConnection();
    void on_pushButton_2_clicked();
    void createImagesFolder();


private:
    Ui::Server *ui;
    QString imagesFolder;
    DatabaseManager*db;
    QTcpServer * tcpServer;
    QMap<int, QTcpSocket*> clientMap;
    QMap<QTcpSocket*, int> socketToClientIdMap;
    QMap<int, QQueue<QJsonObject>> offLineMessages;
    void processData(QTcpSocket*clientSocket, const QByteArray& jsonData);



};

#endif // SERVER_H
