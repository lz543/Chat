#ifndef ADDPANEL_H
#define ADDPANEL_H

#include <QWidget>
#include<QtNetwork>
namespace Ui {
class AddPanel;
}

class AddPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AddPanel(QWidget *parent = nullptr,const int &id=int(),const QString &name=QString(),const int &searchId=int(),const QString &searchName=QString(),QTcpSocket *tcpSocket=nullptr);
    ~AddPanel();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::AddPanel *ui;
    QTcpSocket *tcpSocket;
    int id;
    QString name;
    int searchId;
};

#endif // ADDPANEL_H
