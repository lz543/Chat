


#include "form.h"
#include "ui_form.h"
#include "addpanel.h"
#include "messageItem.h"
#include "friendsitem.h"

#include<QMenu>
#include<QMovie>
#include<QMessageBox>
#include<QGraphicsDropShadowEffect>
Form::Form(QWidget *parent,const int &id,const QString &name,QTcpSocket *tcpSocket,const QString &imagePath,const bool &offmessage):
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    this->id=id;
    this->name=name;
    this->baseFolder=QDir::currentPath() + "/" + QString::number(id);//每个账号文件路径
    this->avatarFolder= this->baseFolder+"/avatar";
    this->imagePath=imagePath;
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    //设置无边框
    this->setWindowFlags(Qt::FramelessWindowHint);
    //实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(0, 0);
    //设置阴影颜色
    shadow->setColor(QColor("#444444"));
    //设置阴影圆角
    shadow->setBlurRadius(10);
    //给嵌套QWidget设置阴影
    ui->widget->setGraphicsEffect(shadow);
    ui->label_4->setStyleSheet(QString("QLabel{border-image: url('%1');border-width:0px;border-radius:40px;border-style:solid;border-color: rgb(255, 255, 255);}").arg(this->imagePath));
    ui->label_5->setText(name);
    ui->label->setText(QString::number(id));
    systemtrayicon = new QSystemTrayIcon(this);
    QIcon icon = QIcon("://1");
    //添加图标
    systemtrayicon->setIcon(icon);
    //当鼠标悬浮，显示文字
    systemtrayicon->setToolTip("Chat");
    systemtrayicon->show();
    menu = new QMenu(this);
    connect(systemtrayicon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason){
        if (reason == QSystemTrayIcon::Trigger) {
            this->show();
            this->activateWindow();;// 将窗口带到最前面
        }
    });
    QAction *m_pShowAction;
    QAction *m_pCloseAction;
    m_pShowAction = new QAction("打开主界面");
    m_pCloseAction = new QAction("退出");
    menu->addAction(m_pShowAction);
    menu->addAction(m_pCloseAction);
    systemtrayicon->setContextMenu(menu);
    QObject::connect(m_pShowAction,&QAction::triggered,this,[=](){
        this->show();
        this->activateWindow();;// 将窗口带到最前面
    });
    QObject::connect(m_pCloseAction,&QAction::triggered,this,[=](){
        this->close();
    });

    // 将菜单设置为系统托盘图标的菜单
    systemtrayicon->setContextMenu(menu);
    //显示图标
    createFolder();//若不存在对应文件夹则创建
    QString path=baseFolder+QDir::separator()+"chat.db";//数据库路径
    db=new DatabaseManager(path,QString::number(id));
    this->setWindowTitle(QString::number(id));//设置窗口名字为账号c
    this->tcpSocket=tcpSocket;  
    this->setAttribute(Qt::WA_DeleteOnClose);//在窗口关闭时自动销毁
    ui->stackedWidget->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->setTabText(0,"消息");
    ui->tabWidget->setTabText(1,"联系人");;
    ui->tabWidget_2->setTabText(0,"好友");
    ui->tabWidget_2->setTabText(1,"群聊");
    connect(this,&Form::friendsFinished,this,[=]{
        connect(tcpSocket, &QTcpSocket::readyRead,this,&Form::onReadyRead);
        messagesInterface();
        if(offmessage){
            QJsonObject message_2;
            message_2["type"] = "Rmessage";
            message_2["userId"] =this->id;
            QJsonDocument jsonDoc_2(message_2);
            this->tcpSocket->write(jsonDoc_2.toJson());
            this->tcpSocket->flush();
        }
        loadFinishedEmitted = true;
        emit loadFinished();
    });
    friendsInterface();
    //双击时打开聊天界面
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &Form::chatObject);
    connect(ui->listWidget_3, &QListWidget::itemDoubleClicked, this, [&](QListWidgetItem *item) {
      int fIdData = item->data(Qt::UserRole).toInt();
      if (fIdData==0) {
        verifyObject();
      } else {
        chatObject(item);
      }
    });
}

Form::~Form()
{
    delete ui;
    ui = nullptr;
    delete systemtrayicon;
    systemtrayicon=nullptr;
    delete menu;
    menu=nullptr;
    db->closeDatabase();
    delete db;
    db=nullptr;
    for (auto it = chatMap.begin(); it != chatMap.end(); ++it) {
        Chat* c = it.value();
        if (c) {
            c->close(); // 关闭 Chat 窗口
            c->deleteLater();
            c=nullptr;// 由 Qt 自动销毁窗口对象
        }
    }
    chatMap.clear();
    if(verify){
        verify->close();
        verify->deleteLater();
        verify=nullptr;
    }
    if (tcpSocket) {
        tcpSocket->disconnectFromHost();
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }
}

//若不存在对应文件夹则创建
void Form::createFolder()
{
    if (!QDir(baseFolder).exists()) {
        QDir().mkdir(baseFolder);
    }
    if (!QDir(avatarFolder).exists()) {
        QDir().mkdir(avatarFolder);
    }
}




//搜索
void Form::on_toolButton_clicked()
{
    QString str=ui->lineEdit->text();
    QJsonObject message;
    message["type"] = "search";
    message["searchId"]=str.toInt();
    QJsonDocument jsonDoc(message);
    QByteArray jsonString = jsonDoc.toJson();// 将 JSON 请求转换为 QByteArray
    // 将字符串发送到服务器
    tcpSocket->write(jsonString);
    tcpSocket->flush();
    tcpSocket->waitForBytesWritten();
}



void Form::on_pushButton_5_clicked()
//搜索后点击返回
{
    ui->label_3->setText(""); // 清空标签的内容
    ui->stackedWidget->setCurrentIndex(0);
}



void Form::on_tabWidget_2_currentChanged(int index)
{
    if(index==0){
        ui->listWidget->update();
    }

}


void Form::on_tabWidget_currentChanged(int index)
{
    if (index == 1) {
        // 切换到 Tab2
        ui->tabWidget_2->setCurrentIndex(0);
        on_tabWidget_2_currentChanged(0); // 显示好友列表项
    }

}


void Form::loadMessageItem(const int &f_id, const QString &f_name, const QString &content, const QString &time)
//加载消息
{
    QString  filePath;//通过id找到头像路径
    if(f_id==0){
        filePath=":/images/laba.png";
    }else{
        filePath=this->avatarFolder+"/"+QString::number(f_id)+".jpg";
    }
    MessageItem *itemWidget = new MessageItem(ui->listWidget_3, filePath,f_name, content, time);
    QListWidgetItem *item=nullptr;
    if (messageMap.contains(f_id)) {
        item = messageMap.value(f_id);
        ui->listWidget_3->removeItemWidget(item);
        delete item;

        // 更新 messageMap，后面会重新创建新的条目
        messageMap.remove(f_id);
    }
    item = new QListWidgetItem();
    item->setData(Qt::UserRole, f_id);
    if (!(f_id == 0)) {
        item->setData(Qt::UserRole + 2, f_name);
    }
    item->setSizeHint(itemWidget->size());
    ui->listWidget_3->insertItem(0, item);  // 将消息放到最上面
    ui->listWidget_3->setItemWidget(item, itemWidget);
    ui->listWidget_3->update();
    messageMap[f_id] = item;
}


void Form::messagesInterface()
{
    if(db->openDatabase())
    {
        QSqlQuery query(db->getDatabase());
        query.prepare("SELECT friendName,MAX(time) AS latest_time FROM message WHERE isVerifyMessage=1");
        if(query.exec() && query.next()){
            QString friendName=query.value("friendName").toString();
            QString latest_time=query.value("latest_time").toString();
            QString content=friendName+"申请加你为好友";
            //当查询不到结果时，因为使用了聚合函数，所以还是有返回结果NULL，用判断非空来解决
            if (!friendName.isEmpty() && !latest_time.isEmpty()) {
                loadMessageItem(0,"验证消息",content,latest_time);
            }

        }
        QSqlQuery query_2(db->getDatabase());
        query_2.prepare("SELECT friendId,friendName, content, MAX(time) AS latest_time FROM message WHERE isVerifyMessage=0  GROUP BY friendId ORDER BY latest_time");
        if(query_2.exec()){
            while(query_2.next()){
                int sendId=query_2.value("friendId").toInt();
                QString sendName=query_2.value("friendName").toString();
                QString content=query_2.value("content").toString();
                QString latest_time=query_2.value("latest_time").toString();
                loadMessageItem(sendId,sendName,content,latest_time);
            }
        }
        db->closeDatabase();
    }
}


void Form::friendsInterface()
{
    QJsonObject message;
    message["type"] = "friends";
    message["id"]=id;
    QJsonDocument jsonDoc(message);
    QByteArray jsonString = jsonDoc.toJson();// 将 JSON 请求转换为 QByteArray
    // 将字符串发送到服务器
    tcpSocket->write(jsonString);
    tcpSocket->flush();
    QByteArray receivedData; // 存储已接收到的数据
    qint64 totalSize = -1; // 总大小
    qint64 receivedBytes = 0; // 已接收的字节数
    while(tcpSocket->waitForReadyRead(1000)){
        if (totalSize == -1) {
            // 接收总大小信息
            QByteArray sizeData = tcpSocket->readAll();
            totalSize = sizeData.toLongLong();
        }
        else {
            // 接收分片消息
            QByteArray chunkData = tcpSocket->readAll();
            receivedData.append(chunkData);
            receivedBytes += chunkData.size();

            // 判断是否已接收完整消息
            if (receivedBytes >= totalSize) {
                processData(receivedData);

                // 重置状态
                receivedData.clear();
                totalSize = -1;
                receivedBytes = 0;
                break;
            }
        }
    }

}

//保存消息到数据库
bool Form::saveMessages(const int &self,const int &friendId,const QString &friendName,const QString &content,const QString &time,const int &isVerifyMessage)
{
    bool success = false;

    if (db->openDatabase()) {
        success = db->insertMessage(self, friendId, friendName, content, time, isVerifyMessage);
        db->closeDatabase();
    }

    return success;
}


//收到消息时
void Form::onReadyRead()
{
    QByteArray jsonData = tcpSocket->readAll();
    processData(jsonData);

}




//处理收到消息的数据
void Form::processData(const QByteArray& jsonData) {
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject message = jsonDoc.object();
    if(message["type"]=="message"){
        //收到的是message类型的消息时
        qDebug()<<"收到了";
        int f_id =message["sender_id"].toInt();
        QString time =message["time"].toString();
        QString content=message["content"].toString();
        QString f_name=message["sender_name"].toString();
        loadMessageItem(f_id,f_name,content,time);
        //判断是否打开好友聊天界面
        if(chatMap.contains(f_id)){
            Chat *c1=chatMap.value(f_id);
            c1->onChatRead(message,time);
        }
        saveMessages(0,f_id,f_name,content,time,0);
    }else if(message["type"]=="friendsResponse"){
        //处理好友分组
        QJsonArray friendsArray=message["friends"].toArray();
        for (const QJsonValue &friendValue : friendsArray) {
            QJsonObject friendObject = friendValue.toObject();
            int friendId = friendObject["friendId"].toInt();
            QString friendIdStr= QString::number(friendId);
            QString friendName = friendObject["friendName"].toString();
            QString f_inf = friendName + "(" + friendIdStr + ")";

            QByteArray imageData = QByteArray::fromBase64(friendObject["imageData"].toString().toUtf8());
            QPixmap pixmap;
            pixmap.loadFromData(imageData, "jpg");
            QString  filePath=this->avatarFolder+"/"+friendIdStr+".jpg";
            if (QFile::exists(filePath)) {
                QFile::remove(filePath);
            }
            // 保存图像数据到文件
            QFile imageFile(filePath);
            if (imageFile.open(QIODevice::WriteOnly)) {
                imageFile.write(imageData);
                imageFile.close();
            }
            QListWidgetItem *item = new QListWidgetItem();
            FriendsItem *itemWidget=new FriendsItem(ui->listWidget,filePath,f_inf);
            item->setSizeHint(itemWidget->size());
            item->setData(Qt::UserRole, friendId);
            item->setData(Qt::UserRole + 2, friendName);
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item, itemWidget);
            ui->listWidget->update();
        }
        emit friendsFinished();
    }else if(message["type"]=="searchResponse"){
        //收到是searchResponse类型，即搜索回应的消息时
        ui->stackedWidget->setCurrentIndex(1);
        if(message["success"].toBool()){
            QString searchName=message["searchName"].toString();
            int searchId=message["searchId"].toInt();
            QString result=searchName+"("+QString::number(searchId)+")";
            ui->label_3->setText(result);
            ui->pushButton_2->show();
            ui->pushButton_2->setEnabled(true);
            //绑定添加按钮
            connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){
                AddPanel *a=new AddPanel(nullptr,this->id,this->name,searchId,searchName,this->tcpSocket);
                a->show();
            });
        }else{
            ui->label_3->setText("没有搜索结果");
            //没有搜索结果时将添加按钮隐藏并禁用
            ui->pushButton_2->hide();
            ui->pushButton_2->setEnabled(false);
        }
    }else if(message["type"]=="add"){
        //处理添加好友
        QString time =message["time"].toString();
        QString remark=message["remark"].toString();
        QString s_name=message["name"].toString();
        int s_id=message["id"].toInt();
        QString content=s_name+"申请加你为好友";
        QByteArray imageData = QByteArray::fromBase64(message["imageData"].toString().toUtf8());
        QPixmap pixmap;
        pixmap.loadFromData(imageData, "jpg");
        QString filePath=this->avatarFolder+"/"+QString::number(s_id)+".jpg";
        // 保存图像数据到文件
        QFile imageFile(filePath);
        if (imageFile.open(QIODevice::WriteOnly)) {
            imageFile.write(imageData);
            imageFile.close();
        }
        saveMessages(0,s_id,s_name,remark,time,1);
        loadMessageItem(0,"验证消息",content,time);
    }else if(message["type"]=="addResponse"){
        //处理添加好友结果
        QString time =message["time"].toString();
        QString addName=message["addName"].toString();
        int addId=message["addId"].toInt();
        int success=message["success"].toInt();
        QString content="我同意了你的好友申请，快来和我聊天吧";
        if(success==1){
            //好友列表更新
            QByteArray imageData = QByteArray::fromBase64(message["imageData"].toString().toUtf8());
            QPixmap pixmap;
            pixmap.loadFromData(imageData, "jpg");
            QString filePath=this->avatarFolder+"/"+QString::number(addId)+".jpg";
            // 保存图像数据到文件
            QFile imageFile(filePath);
            if (imageFile.open(QIODevice::WriteOnly)) {
                imageFile.write(imageData);
                imageFile.close();
            }
            QString f_inf=addName+"("+QString::number(addId)+")";
            QListWidgetItem *item = new QListWidgetItem();
            FriendsItem *itemWidget=new FriendsItem(ui->listWidget,filePath,f_inf);
            item->setSizeHint(itemWidget->size());
            item->setData(Qt::UserRole, addId);
            item->setData(Qt::UserRole + 2, addName);
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item, itemWidget);
            ui->listWidget->update();
            //消息列表更新
            loadMessageItem(addId,addName,content,time);
            saveMessages(0,addId,addName,content,time,0);//保存消息
        }
    }
}

void Form::verifyObject()
{

    verify=new VerifyMessage(nullptr,db);
    connect(verify,&VerifyMessage::verifySignal,this,[=](int isAccept,int sendId,QString sendName){
        QDateTime currentTime = QDateTime::currentDateTime();
        QString time = currentTime.toString("yyyy-MM-dd hh:mm:ss");
        // 构建包含ID信息的JSON对象
        QJsonObject message;
        message["type"] = "addResponse";
        message["time"] = time;
        message["name"]=this->name;
        message["accept?"]=isAccept;
        message["sendId"]=sendId;
        QJsonDocument jsonDoc(message);
        QByteArray jsonString = jsonDoc.toJson();// 将 JSON 请求转换为 QByteArray
        // 将字符串发送到服务器
        this->tcpSocket->write(jsonString);
        this->tcpSocket->flush();
        if(isAccept==1){
            QString content="我同意了你的好友申请，快来和我聊天吧";
              loadMessageItem(sendId,sendName,content,time);
            QString filePath=this->avatarFolder+"/"+QString::number(sendId)+".jpg";
            QString f_inf=sendName+"("+QString::number(sendId)+")";
            QListWidgetItem *item = new QListWidgetItem();
            FriendsItem *itemWidget=new FriendsItem(ui->listWidget,filePath,f_inf);
            item->setSizeHint(itemWidget->size());
            item->setData(Qt::UserRole, sendId);
            item->setData(Qt::UserRole + 2, sendName);
            ui->listWidget->addItem(item);
            ui->listWidget->setItemWidget(item, itemWidget);
            ui->listWidget->update();
            saveMessages(1,sendId,sendName,content,time,0);
        }
        if(db->openDatabase()){
            QSqlQuery query(db->getDatabase());
            query.prepare("UPDATE message SET self=:self,time=:time WHERE time =(SELECT MAX(time) FROM message WHERE friendId=:friendId AND isVerifyMessage=1)");
            query.bindValue(":self",3-isAccept);
            query.bindValue(":time",time);
            query.bindValue(":friendId",sendId);
            if(!query.exec()){
                qDebug()<<"更新出错";
            }
            db->closeDatabase();
        }
    });
    verify->show();

}


void Form::chatObject(QListWidgetItem* item)
{
    int chatId = item->data(Qt::UserRole).toInt();

    // 检查是否已经打开了聊天窗口
    if (chatMap.contains(chatId)) {
         //将窗口带到最前面
        Chat* c = chatMap.value(chatId);
        c->activateWindow(); // 激活窗口
        c->raise(); // 将窗口带到最前面
    } else {
        // 新建聊天窗口并保存窗口实例
        QString f_name=item->data(Qt::UserRole+2).toString();
        Chat* c = new Chat(nullptr,id,name,chatId,f_name,this->db,this->tcpSocket,this->imagePath);
        c->setAttribute(Qt::WA_DeleteOnClose); // 设置窗口在关闭时自动销毁
        connect(c,&Chat::updateMessage,this,[=](){
            if(db->openDatabase()){
                QSqlQuery query(db->getDatabase());
                query.prepare("SELECT friendId,friendName,content,time from user ORDER BY time DESC LIMIT 1");
                if(query.exec()&&query.next()){
                    int f_id=query.value("friendId").toInt();
                    QString f_name=query.value("friendName").toString();
                    QString content=query.value("content").toString();
                    QString time=query.value("time").toString();
                    loadMessageItem(f_id,f_name,content,time);
                    db->closeDatabase();
                }
            }
        });
        chatMap[c->f_id]=c;
        connect(c, &Chat::destroyed, this, [=](){
            chatMap.remove(c->f_id);
        });//当关闭chat对象时，将chatList中id移除
        c->show();
    }
}




void Form::on_toolButton_2_clicked()
{
    this->hide();
}


void Form::on_toolButton_3_clicked()
{
    this->close();
}

