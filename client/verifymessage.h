#ifndef VERIFYMESSAGE_H
#define VERIFYMESSAGE_H

#include"databasemanager.h"
#include <QWidget>
#include<QtNetwork>
#include<QSqlDatabase>
#include<QListWidgetItem>
namespace Ui {
class VerifyMessage;
}

class VerifyMessage : public QWidget
{
    Q_OBJECT

public:
    explicit VerifyMessage(QWidget *parent = nullptr,DatabaseManager *db=nullptr);
    ~VerifyMessage();
    void verifyInterface();

signals:
    void verifySignal(int isAccept,int f_id,QString f_name);


private:
    Ui::VerifyMessage *ui;
    DatabaseManager *db;

    //QString name;


};

#endif // VERIFYMESSAGE_H
