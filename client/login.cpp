
#include "login.h"
#include "ui_login.h"
#include "form.h"
#include<QMenu>
#include<QMovie>
#include<QString>
#include<QtNetwork>
#include<QLineEdit>
#include<QPushButton>
#include<QMessageBox>
#include<QFileDialog>
#include<QGraphicsDropShadowEffect>

#define ADDRESS "192.168.0.104"
#define PORT 8888

qt4::qt4(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::qt4)
{
    ui->setupUi(this);
    this->imagePath=QDir::currentPath() + "/images";
    this->filename=":/images/kong.jpg";
    createFolder();
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
    ui->stackedWidget->setGraphicsEffect(shadow);
    ui->stackedWidget->setCurrentIndex(0);

    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){this->close();});
    QMovie *movie = new QMovie(":/images/bb.gif");
    movie->setScaledSize(QSize(580,140));
    ui->label_4->setMovie(movie);
    ui->label_5->setMovie(movie);
    movie->setScaledSize(QSize(580,410));
    ui->label->setMovie(movie);
    movie->start();
    QAction * action = new QAction(ui->lineEdit);
    action->setIcon(QIcon(":/1"));
    ui->lineEdit->addAction(action,QLineEdit::LeadingPosition);//表示action所在方位（左侧）
    QAction * action_2 = new QAction(ui->lineEdit);
    action_2->setIcon(QIcon(":/images/p.png"));
    ui->lineEdit_2->addAction(action_2,QLineEdit::LeadingPosition);//表示action所在方位（左侧）

}

qt4::~qt4()
{
    delete ui;
    ui = nullptr;
}


void qt4::createFolder()
//创建文件夹
{
    if (!QDir(imagePath).exists()) {
        QDir().mkdir(imagePath);
    }
}


void qt4::on_pushButton_clicked()
//登录按钮
{
    int str=ui->lineEdit->text().toInt();
    QString str2=ui->lineEdit_2->text();
    if(str<10000000||str>999999999){
        QMessageBox::information(this, "提示", "账号长度输入有误");
    }
    else if(str2.length()<8||str2.length()>20){
        QMessageBox::information(this, "提示", "密码长度输入有误");
    }
    else {
        ui->stackedWidget->setCurrentIndex(2);
        QTcpSocket *socket= new QTcpSocket();
        socket->connectToHost(ADDRESS, PORT);
        socket->waitForConnected(3000);
        if (socket->state()==QAbstractSocket::ConnectedState){
            QJsonObject message;
            message["type"] = "login";
            message["userId"] = str;
            message["password"] = str2;
            QJsonDocument jsonDoc(message);
            QByteArray jsonString = jsonDoc.toJson();// 将 JSON 请求转换为 QByteArray
            // 将字符串发送到服务器
            socket->write(jsonString);
            socket->flush();
            socket->waitForBytesWritten();
            //等待服务器验证
            if (socket->waitForReadyRead()) {
                QByteArray response = socket->readAll();
                QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
                QJsonObject responseObject = jsonResponse.object();
                if (responseObject["type"] == "loginResponse"){
                    bool success = responseObject["success"].toBool();
                    if (success) {
                        // 收到成功响应消息,判断是否有离线消息
                        bool offmessage = responseObject["offlineMessages"].toBool();
                        //用户昵称
                        QString name=responseObject["name"].toString();
                        QByteArray imageData = QByteArray::fromBase64(responseObject["imageData"].toString().toUtf8());
                        QPixmap pixmap;
                        pixmap.loadFromData(imageData, "jpg");

                        // 保存图像数据到文件
                        QString time = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
                        QString filePath = this->imagePath+"/"+ QString::number(str)+"_"+time+".jpg";//头像路径
                        QFile imageFile(filePath);
                        if (imageFile.open(QIODevice::WriteOnly)) {
                            imageFile.write(imageData);
                            imageFile.close();
                        }
                        Form * new1=new Form(nullptr,str,name,socket,filePath,offmessage);
                        if (new1->loadFinishedEmitted) {
                            this->close();
                            new1->show();
                        } else {
                            connect(new1, &Form::loadFinished, this, [=]() {
                                this->close();
                                new1->show();
                            });
                        }
                    } else {
                        ui->stackedWidget->setCurrentIndex(0);
                        socket->disconnect();
                        socket->deleteLater();
                        socket=nullptr;
                        QString reason=responseObject["reason"].toString();
                        QMessageBox::information(this, "提示", reason);
                    }
                }else{
                    ui->stackedWidget->setCurrentIndex(0);
                    QMessageBox::information(this, "提示", "服务器开小差了");
                    socket->disconnect();
                    socket->deleteLater();
                    socket=nullptr;
                }
            }
        }else{
            ui->stackedWidget->setCurrentIndex(0);
            QMessageBox::information(this, "提示", "无法连接到服务器");
            socket->disconnect();
            socket->deleteLater();
            socket=nullptr;
        }
    }

}

void qt4::on_pushButton_2_clicked()
//登录界面点击退出
{
    this->close();
}


void qt4::on_toolButton_clicked()
//点击缩小
{
    this->hide();
}


void qt4::on_toolButton_2_clicked()
//点击关闭
{
    this->close();
}


void qt4::on_pushButton_3_clicked()
//登录界面点击注册
{
    ui->stackedWidget->setCurrentIndex(1);
}

void qt4::on_pushButton_4_clicked()
//注册界面点击注册
{
    QString s=ui->lineEdit_3->text();
    QString s2=ui->lineEdit_4->text();
    if(s.isEmpty()){
        QMessageBox::information(this, "提示", "昵称不为空");
    }
    else if(s.length()>30){
        QMessageBox::information(this, "提示", "昵称过长");
    }
    else if(s2.length()<8||s2.length()>20){
        QMessageBox::information(this, "提示", "密码长度不得低于8位且不得高于20位");
    }
    else{
        QTcpSocket *socket=new QTcpSocket(this);
        socket->connectToHost(ADDRESS, PORT);
        //判断是否连接到服务器
        if (socket->waitForConnected(3000)){
            QJsonObject message;
            QPixmap pixmap(this->filename);
            QByteArray imageData;
            QBuffer buffer(&imageData);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "jpg",50);
            buffer.close(); // 关闭缓冲区以刷新数据
            message["type"] = "enroll";
            message["enrollName"]=s;
            message["enrollPassword"]=s2;
            message["imageData"] =QString(imageData.toBase64());
            QJsonDocument jsonDoc(message);
            QByteArray jsonString = jsonDoc.toJson();// 将 JSON 请求转换为 QByteArray        
            // 将字符串发送到服务器
            QAbstractSocket::SocketOption option = QAbstractSocket::SendBufferSizeSocketOption;
            qint64 bufferSize = socket->socketOption(option).toLongLong();
            qDebug() << "Socket buffer maximum size: " << bufferSize;
            qDebug()<<jsonString.size();
            if (!(bufferSize<jsonString.size())) {
                socket->write(jsonString);
                socket->flush();
                qDebug()<<"发送消息了";
                //等待接收服务器的响应
                if (socket->waitForReadyRead(3000)) {
                    qDebug()<<"收到消息了";
                    QByteArray response = socket->readAll();
                    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
                    QJsonObject responseObject = jsonResponse.object();
                    if (responseObject["type"] == "enrollResponse"){
                        bool success = responseObject["success"].toBool();
                        if(success){
                            QString id=responseObject["id"].toString();
                            //QMessageBox::information(this, "账号"+id+"注册成功","2s后自动切换页面");
                            QTimer *timer = new QTimer(this);
                            // 连接槽函数，延时结束后执行请求离线消息操作
                            connect(timer, &QTimer::timeout, this, [=](){
                                ui->stackedWidget->setCurrentIndex(0);
                                ui->lineEdit->setText(id);
                                timer->stop();
                                timer->deleteLater();
                            });
                            // 启动定时器，等待一段时间（例如2000毫秒，即2秒）
                            timer->start(2000);
                        }else{
                            QMessageBox::information(this, "提示", "服务器开小差了");
                        }
                    }
                }
            }else{
                QMessageBox::information(this, "提示", "图片太大了，换一张吧");
            }
            socket->disconnectFromHost();
        }else{
            QMessageBox::information(this, "提示", "无法连接到服务器");
        }
        socket->deleteLater();
        socket=nullptr;
    }
}


void qt4::on_toolButton_3_clicked()
{
    this->hide();
}


void qt4::on_toolButton_4_clicked()
{
    this->hide();
}


void qt4::on_pushButton_5_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void qt4::on_toolButton_5_clicked()
{
    this->filename = QFileDialog::getOpenFileName(nullptr, "选择图片文件", "", "图片文件 (*.jpg *.png)");
    if (!filename.isEmpty()) {
        ui->label_2->setStyleSheet(QString("QLabel{border-image: url('%1');border-width:0px;border-radius:45px;border-style:solid;border-color: rgb(255, 255, 255);}").arg(filename));
    }else{
        this->filename= this->imagePath+"/kong.jpg";
    }

}

