
#include "chatitem.h"

#include<QBoxLayout>
ChatItem::ChatItem(QWidget *parent ,const QString& time,const QString& avatarPath,const QString& message,bool isSelf):
    QWidget(parent)
{
    this->setMinimumHeight(100);
    this->setFixedWidth(666);//设置宽度
    avatarLabel=new QLabel(this);
    avatarLabel->setFixedSize(40, 40);
    avatarLabel->setStyleSheet(QString("QLabel{border-image: url('%1');border-radius:20px;}").arg(avatarPath));//设置头像

    messageLabel=new QLabel(message);
    messageLabel->setStyleSheet("QLabel{color: rgb(0,0,0);font: 290 10pt 'Microsoft YaHei';background-color: rgb(235,235,235);border-radius: 10px 10px 10px 10px; border: 1px solid rgb(255,255,255);padding: 10px;}");
    int contentWidth = messageLabel->sizeHint().width();
    int contentHeight = messageLabel->sizeHint().height();

    // 设置消息框的最小尺寸，根据内容自动调整大小
    int minWidth = 40; // 设置消息框的最小宽度
    int maxWidth = 400; // 设置消息框的最大宽度
    int minHeight = 40; // 设置消息框的最小高度
    int messageWidth;
    if(contentWidth<minWidth){
        messageWidth=minWidth;
    }else if(contentWidth>maxWidth){
        messageWidth=maxWidth;
    }else{
        messageWidth=contentWidth;
    }

    int messageHeight = qMax(minHeight, contentHeight);

    messageLabel->setFixedSize(messageWidth, messageHeight);
    messageLabel->setWordWrap(true); // 设置自动换行
    timeLabel=new QLabel(time);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet("QLabel{color: rgb(0,0,0);font: 290 8pt 'Microsoft YaHei';}");
    QBoxLayout *hlayout=new QHBoxLayout();
    if(isSelf){
        hlayout->addWidget(avatarLabel);
        hlayout->addWidget(messageLabel);
        hlayout->addStretch();
        avatarLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);

    }else{
        hlayout->addStretch();
        hlayout->addWidget(messageLabel);
        hlayout->addWidget(avatarLabel);
        avatarLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    }
    hlayout->setContentsMargins(5,5,5,5);



    QBoxLayout *vlayout=new QVBoxLayout();
    vlayout->addWidget(timeLabel);
    vlayout->addLayout(hlayout);
    this->setLayout(vlayout);

}

