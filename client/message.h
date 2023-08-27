
#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
//保存在文件中的格式
struct Message {
    QString self;//是否是自己发送，1表示自己发送给好友的，0则相反
    QString time;//消息发送时间
    QString content;//消息内容
    QString f_name;
};

#endif // MESSAGE_H
