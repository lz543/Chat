#ifndef VERIFYITEM_H
#define VERIFYITEM_H

#include <QWidget>
#include<QLabel>
#include<QtNetwork>
#include<QPushButton>
namespace Ui {
class VerifyItem;
}

class VerifyItem : public QWidget
{
    Q_OBJECT

public:
    void updateRemark(const QString &remark=QString());
    void updateTime(const QString &time=QString());
    void updateUI(const int &index);
    explicit VerifyItem(QWidget *parent = nullptr,const int &f_id=int(),const QString &f_name=QString(),const QString &remark=QString(),const QString &time=QString(),const int &index=int());
    ~VerifyItem();

signals:
    void idSignal(int isAccept,int id,QString f_name);



private:
    Ui::VerifyItem *ui;
    int f_id;
    QPushButton *button_1;
    QPushButton *button_2;
    QLabel *label;


};

#endif // VERIFYITEM_H
