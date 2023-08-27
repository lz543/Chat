#ifndef FORM_H
#define FORM_H

#include"chat.h"
#include "verifymessage.h"
#include "databasemanager.h"
#include<QMap>
#include <QWidget>
#include<QtNetwork>
#include<QPoint>
#include<QMouseEvent>
#include<QVBoxLayout>
#include<QSqlDatabase>
#include<QSystemTrayIcon>
namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    bool loadFinishedEmitted = false;
    QMap<int,QListWidgetItem*> messageMap;//消息列表的好友id
    QMap<int, Chat*> chatMap;//保存打开的chat对象对应的id
    QTcpSocket *tcpSocket;
    void processData(const QByteArray& jsonData);
    void createFolder();
    void onReadyRead();
    void messagesInterface();
    void friendsInterface();
    void verifyObject();
    bool saveMessages(const int &self,const int &friendId,const QString &friendName,const QString &content,const QString &time,const int &isVerifyMessage);
    explicit Form(QWidget *parent = nullptr,const int &id=0,const QString &name=QString(),QTcpSocket *tcpSocket=nullptr,const QString &imagePath=QString(),const bool &offmessage=bool());
    ~Form();

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            m_dragPosition = event->globalPosition().toPoint() - geometry().topLeft();
            event->accept();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (event->buttons() & Qt::LeftButton)
        {
            move(event->globalPosition().toPoint() - m_dragPosition);
            event->accept();
        }
    }



private slots:

    void on_toolButton_clicked();

    void on_pushButton_5_clicked();

    void on_tabWidget_2_currentChanged(int index);

    void on_tabWidget_currentChanged(int index);


    void chatObject(QListWidgetItem* item);

    void loadMessageItem(const int &f_id, const QString &f_name, const QString &content, const QString &time);

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();

private:
    Ui::Form *ui;
    QPoint m_dragPosition;
    int id;//账号
    QString name;//名字
    DatabaseManager *db;
    QString baseFolder;
    QString imagePath;
    QString avatarFolder;
    QSystemTrayIcon  * systemtrayicon;//系统托盘
    QMenu *menu;//托盘菜单
    VerifyMessage *verify=nullptr;//验证消息界面
    QQueue<QByteArray> messageQueue;  // 消息队列
    bool processingMessages = false;


signals:
    //void signalToC();
    void loadFinished();
    void friendsFinished();

};


#endif // FORM_H
