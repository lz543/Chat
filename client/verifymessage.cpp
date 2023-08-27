#include "verifymessage.h"
#include "ui_verifymessage.h"
#include "verifyitem.h"
#include<QSqlQuery>
VerifyMessage::VerifyMessage(QWidget *parent,DatabaseManager *db) :
    QWidget(parent),
    ui(new Ui::VerifyMessage)
{
    ui->setupUi(this);
    this->setWindowTitle("验证消息");
    this->db=db;
    verifyInterface();

}

VerifyMessage::~VerifyMessage()
{
    delete ui;
}

void VerifyMessage::verifyInterface()
{
    if(db->openDatabase()){
        QSqlQuery query(db->getDatabase());
        query.prepare("SELECT self,friendId,friendName,content,MAX(time) as latest_time FROM message WHERE isVerifyMessage=1 GROUP BY friendId");
        if(query.exec()){
            while(query.next()){
                int self=query.value("self").toInt();
                int sendId=query.value("friendId").toInt();
                QString sendName=query.value("friendName").toString();
                QString remark=query.value("content").toString();
                QString latest_time=query.value("latest_time").toString();

                VerifyItem *itemWidget=new VerifyItem(nullptr,sendId,sendName,remark,latest_time,self);
                connect(itemWidget,&VerifyItem::idSignal,this,[=](int isAccept,int f_id,QString f_name){
                         qDebug()<<"收到了"<<isAccept<<" "<<f_id<<" "<<f_name;
                         emit verifySignal(isAccept,f_id,f_name);
                });
                QListWidgetItem *item = new QListWidgetItem();
                item->setSizeHint(itemWidget->size());
                item->setData(Qt::UserRole, QVariant::fromValue(itemWidget));
                ui->listWidget->addItem(item);
                ui->listWidget->setItemWidget(item,itemWidget);
                ui->listWidget->update();

            }
        }
        db->closeDatabase();
    }

}

