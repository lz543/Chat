
#ifndef FRIENDSITEM_H
#define FRIENDSITEM_H

#include <QWidget>
#include<Qlabel>


class FriendsItem: public QWidget
{
    Q_OBJECT
public:
    FriendsItem(QWidget *parent = nullptr,const QString& avatarPath=QString(),const QString& information=QString());

private:
    QLabel* avatarLabel;//头像
    QLabel* informationLabel;//名字
};

#endif // FRIENDSITEM_H
