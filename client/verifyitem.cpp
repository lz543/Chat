#include "verifyitem.h"
#include "ui_verifyitem.h"

VerifyItem::VerifyItem(QWidget *parent,const int &f_id,const QString &f_name,const QString &remark,const QString &time,const int &index) :
    QWidget(parent),
    ui(new Ui::VerifyItem)
{
    ui->setupUi(this);
    this->f_id=f_id;
    this->setWindowTitle("验证消息");
    ui->label->setText(f_name);
    ui->label_2->setText("附加消息:"+remark);
    ui->label_3->setText(time);
    ui->label_2->setWordWrap(true);//文本过长时自动换行
    button_1=new  QPushButton("同意",this);
    button_2=new  QPushButton("拒绝",this);
    button_1->setGeometry(350,45,90,30);
    button_2->setGeometry(460,45,90,30);
    connect(button_1, &QPushButton::clicked, this,[=](){
        qDebug()<<"点击按钮发送信号";
        emit idSignal(1,f_id,f_name);
        updateUI(2);
    } );
    connect(button_2, &QPushButton::clicked, this, [=](){
        qDebug()<<"点击按钮发送信号";
        emit idSignal(0,f_id,f_name);
        updateUI(3);
    } );
    label=new QLabel(this);
    label->setAlignment(Qt::AlignCenter); // 设置文本内容居中显示
    label->setFixedSize(90, 30);
    label->move(460,45);
    updateUI(index);



}

VerifyItem::~VerifyItem()
{
    delete ui;
    ui=nullptr;
    delete button_1;
    delete button_2;
    delete label;
}

void VerifyItem::updateUI(const int &index) {
    if (index == 0) {
        button_1->show();
        button_2->show();
        label->hide();
    } else if (index == 2) {
        button_1->hide();
        button_2->hide();
        label->setText("已同意");
        label->show();
    } else if (index == 3) {
        button_1->hide();
        button_2->hide();
        label->setText("已拒绝");
        label->show();
    }
}

void VerifyItem::updateRemark(const QString &remark)
{
    ui->label_2->setText(remark);
}

void VerifyItem::updateTime(const QString &time)
{
    ui->label_3->setText(time);
}
