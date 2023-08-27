#ifndef MESSAGEITEM_H
#define MESSAGEITEM_H

#include <QWidget>
#include<QLabel>


class MessageItem : public QWidget
{
    Q_OBJECT

public:
    explicit MessageItem(QWidget *parent = nullptr,const QString& avatarPath=QString(),const QString& name=QString(), const QString& content=QString(), const QString& time=QString());


private:
    QLabel* avatarLabel;//头像
    QLabel* nameLabel;//名字
    QLabel* contentLabel;//内容
    QLabel* timeLabel;//时间
};

#endif // MESSAGEITEM_H
