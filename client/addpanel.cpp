#include "addpanel.h"
#include "ui_addpanel.h"
#include<QPainter>
#include<QMessageBox>
AddPanel::AddPanel(QWidget *parent,const int &id,const QString &name,const int &searchId,const QString &searchName,QTcpSocket *tcpSocket) :
    QWidget(parent),
    ui(new Ui::AddPanel)
{
    ui->setupUi(this);
    this->setWindowTitle("添加好友");
    this->tcpSocket=tcpSocket;
    this->id=id;
    this->name=name;
    this->searchId=searchId;
    ui->label->setText("昵称:"+searchName);
    ui->label_2->setText("账号:"+QString::number(searchId));
    QFont font;
    font.setBold(true);
    ui->label->setFont(font);
    ui->label_2->setFont(font);
}

AddPanel::~AddPanel()
{
    delete ui;
    ui = nullptr;
}

void AddPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // 设置绘制区域的背景色
    painter.fillRect(0, 0, 180, 450, QColor(170, 255, 255)); // 设置颜色

}

void AddPanel::on_pushButton_2_clicked()
{
    this->close();
}


void AddPanel::on_pushButton_clicked()
{
    QString remark=ui->textEdit->toPlainText();
    QDateTime currentTime = QDateTime::currentDateTime();
    QString time = currentTime.toString("yyyy-MM-dd hh:mm:ss");
    QJsonObject message;
    message["type"] = "add";
    message["id"]=this->id;
    message["name"]=this->name;
    message["time"]=time;
    message["addId"]=this->searchId;
    message["remark"]=remark;
    QJsonDocument jsonDoc(message);
    QByteArray jsonString = jsonDoc.toJson();// 将 JSON 请求转换为 QByteArray
    // 将字符串发送到服务器
    tcpSocket->write(jsonString);
    tcpSocket->flush();
    QMessageBox::information(this, "提示", "发送好友申请成功");
}

