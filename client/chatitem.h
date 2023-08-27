
#ifndef CHATITEM_H
#define CHATITEM_H

#include <QWidget>
#include<QLabel>

class ChatItem:public QWidget
{
    Q_OBJECT
public:
    ChatItem(QWidget *parent = nullptr,const QString& time=QString(),const QString& avatarPath=QString(),const QString& message=QString(),bool isSelf=bool());

private:
    QLabel *timeLabel;
    QLabel *avatarLabel;
    QLabel *messageLabel;
    bool isSelf;//1表示自己发送的
};

#endif // CHATITEM_H
