#include "chat.h"
#include "ui_chat.h"
#include "chatitem.h"
#include<QLabel>
#include<QMessageBox>
#include<QKeyEvent>
#include<QDateTime>
Chat::Chat(QWidget *parent ,int id,const QString &name,int f_id,const QString &f_name,DatabaseManager *db,QTcpSocket*socket,const QString &avatarPath) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    this->id=id;
    this->name=name;
    this->f_id=f_id;
    this->f_name=f_name;
    this->db=db;
    this->socket=socket;
    this->avatarPath=avatarPath;
    this->f_avatarPath=QDir::currentPath() + "/" + QString::number(id)+"/avatar/"+QString::number(this->f_id)+".jpg";
    this->setWindowTitle(this->f_name);
    ui->textEdit->setFocus();
    ui->textEdit->installEventFilter(this);//设置完后自动调用其eventFilter函数
    ui->toolButton->setIcon(QIcon("://1"));
    ui->toolButton_2->setIcon(QIcon("://2"));
    ui->toolButton_3->setIcon(QIcon("://3"));
    loadMessages();


}

Chat::~Chat()
{
    delete ui;
    ui = nullptr;
}




void Chat::on_pushButton_clicked()
{
    if(!ui->textEdit->toPlainText().isEmpty()){
        QString str=ui->textEdit->toPlainText();
        ui->textEdit->clear();
        QDateTime currentTime = QDateTime::currentDateTime();
        QString time = currentTime.toString("yyyy-MM-dd hh:mm:ss");
        // 构建包含ID信息的JSON对象
        QJsonObject message;
        message["type"] = "message";
        message["time"] = time;
        message["sender_id"] = id;
        message["sender_name"]=name;
        message["receiver_id"] = f_id;
        message["content"] = str;

        // 将JSON对象转换为字符串
        QJsonDocument jsonDoc(message);
        QString jsonString = jsonDoc.toJson();

        // 将字符串发送到服务器
        socket->write(jsonString.toUtf8());
        socket->flush();
        QListWidgetItem* item=new QListWidgetItem();
        ChatItem *widget=new ChatItem(ui->listWidget,time,this->avatarPath,str,1);
        item->setSizeHint(widget->size()); // 设置项的大小
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, widget);
        saveMessages(f_id,f_name,str,time);
        emit updateMessage();
    }
}

//按下enter键发送消息
bool Chat::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->textEdit)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
            QKeyEvent *k = static_cast<QKeyEvent *>(event);
            if(k->key() == Qt::Key_Return)
            {
                on_pushButton_clicked();
                return true;
            }
        }
    }
    return QWidget::eventFilter(target,event);
}


// 保存消息到数据库
void Chat::saveMessages(const int &friendId,const QString &friendName,const QString &content,const QString &time) {
    if (db->openDatabase()) {
        db->insertMessage(1, friendId, friendName, content, time,0);
        db->closeDatabase();
    }

}

// 加载以前的消息
void Chat::loadMessages() {
    if(db->openDatabase()){
        QSqlQuery query(db->getDatabase());
        query.prepare("SELECT self,content,time FROM message WHERE friendId=:friendId AND isDelete=0 AND isVerifyMessage=0 ORDER BY time");
        query.bindValue(":friendId", this->f_id);
        if(query.exec()){
            while(query.next()){
                int self=query.value("self").toInt();
                QString content=query.value("content").toString();
                QString time=query.value("time").toString();
                QListWidgetItem* item=new QListWidgetItem();
                ChatItem *widget;
                if(self==1){
                    widget=new ChatItem(ui->listWidget,time,this->avatarPath,content,1);

                }else{
                    widget=new ChatItem(ui->listWidget,time,this->f_avatarPath,content,0);
                }
                item->setSizeHint(widget->size()); // 设置项的大小
                ui->listWidget->addItem(item);
                ui->listWidget->setItemWidget(item, widget); // 将自定义小部件设置为项的小部件
            }
        }
        ui->listWidget->scrollToBottom(); // 滚动视图到底部
        db->closeDatabase();
    }

}

void Chat::onChatRead(QJsonObject message,QString time)
{
    QString content=message["content"].toString();
    QListWidgetItem* item=new QListWidgetItem();
    ChatItem *widget=new ChatItem(ui->listWidget,time,this->f_avatarPath,content,0);
    item->setSizeHint(widget->size()); // 设置项的大小
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, widget);

}
