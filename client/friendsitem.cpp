
#include "friendsitem.h"
#include<QBoxLayout>
FriendsItem::FriendsItem(QWidget *parent ,const QString& avatarPath,const QString& information):
    QWidget(parent)
{
    this-> setFixedSize(337,80);

    avatarLabel = new QLabel();
    avatarLabel->setFixedSize(70,70);
    avatarLabel->setStyleSheet(QString("QLabel{border-image: url('%1');border-width:0px;border-radius:35px;border-style:solid;border-color: rgb(255, 255, 255);}").arg(avatarPath));
    informationLabel = new QLabel(information);
    informationLabel->setStyleSheet("QLabel{background-color: transparent;font: 10pt '黑体';}");
    QBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(avatarLabel);
    hLayout->addWidget(informationLabel);
    hLayout->setContentsMargins(5, 5, 5, 5);
    this->setLayout(hLayout);
    this->setStyleSheet("FriendsItem{ background-color: #f0f0f0}");

}

