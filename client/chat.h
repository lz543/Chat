#ifndef CHAT_H
#define CHAT_H

#include "databasemanager.h"
#include<QList>
#include <QWidget>
#include<QtNetwork>
#include <QListWidgetItem>
namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    int id;
    QString name;
    int f_id;
    QString f_name;
    QTcpSocket *socket;
    void onChatRead(QJsonObject message,QString time);
    explicit Chat(QWidget *parent = nullptr,int id=0,const QString &name=QString(),int f_id=0,const QString &f_name=QString(),DatabaseManager *db=nullptr,QTcpSocket*socket=nullptr,const QString &avatarPath=QString());
    void saveMessages(const int &friendId,const QString &friendName,const QString &content,const QString &time) ;
    void loadMessages() ;
    void saveCurrentMessage(const QString& message) ;
    ~Chat();



private slots:
    void on_pushButton_clicked();


protected:
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器


\
private:
    Ui::Chat *ui;
    DatabaseManager *db;
    QString avatarPath;//自己的头像路径
    QString f_avatarPath;//好友头像路径


signals:
    void updateMessage();

};


#endif // CHAT_H
