#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include<QtNetwork>
#include<QThreadPool>
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
    //void onReadyRead();
    void onNewConnection();
    void on_pushButton_2_clicked();
    void createImagesFolder();


private:
    Ui::Server *ui;
    QString imagesFolder;
    QThreadPool threadPool;
    QTcpServer * tcpServer;
    QByteArray imageToByteArray(const QString &imagePath, int quality = 50);
    void processMessage(const QJsonObject& message);
    void processAdd(const QJsonObject& message);
    void processLogin(QTcpSocket* clientSocket, const QJsonObject& message);
    void processEnroll(QTcpSocket* clientSocket, const QJsonObject& message);
    void processFriends(QTcpSocket* clientSocket, const QJsonObject& message);
    void processSearch(QTcpSocket* clientSocket, const QJsonObject& message);
    void processRmessage(QTcpSocket* clientSocket, const QJsonObject& message);
    void processAddResponse(QTcpSocket* clientSocket, const QJsonObject& message);
};

#endif // SERVER_H
