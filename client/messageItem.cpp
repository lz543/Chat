#include "messageitem.h"

#include<QLabel>
#include<QBoxLayout>
MessageItem::MessageItem(QWidget *parent,const QString& avatarPath,const QString& name, const QString& content, const QString& time) :
    QWidget(parent)
{
    this-> setFixedSize(337,80);
    this->setStyleSheet("MessageItem { background-color: #f0f0f0; border: 1px solid #ccc; padding: 5px; }");
    avatarLabel = new QLabel();
    avatarLabel->setFixedSize(70,70);
    avatarLabel->setStyleSheet(QString("QLabel{border-image: url('%1');border-width:0px;border-radius:35px;border-style:solid;border-color: rgb(255, 255, 255);}").arg(avatarPath));

    nameLabel = new QLabel(name);
    QFont font;
    font.setPointSize(10);
    font.setWeight(QFont::Bold); // 设置为黑体
    nameLabel->setFont(font);
    nameLabel->setStyleSheet("QLabel{background-color: transparent;}");
    contentLabel = new QLabel(content);
    contentLabel->setStyleSheet("QLabel{background-color: transparent;}");
    timeLabel = new QLabel(time);
    timeLabel->setStyleSheet("QLabel{background-color: transparent;}");
    QBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(nameLabel);
    hLayout->addWidget(timeLabel);
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(contentLabel);
    QBoxLayout* messageLayout = new QHBoxLayout();
    messageLayout->addWidget(avatarLabel);
    messageLayout->addLayout(vLayout);
    this->setLayout(messageLayout);
}

