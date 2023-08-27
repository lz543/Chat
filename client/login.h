
#ifndef LOGIN_H
#define LOGIN_H

#include<QtNetwork>
#include <QWidget>
#include<QPoint>
#include<QPainter>
#include<QMouseEvent>
#include<QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class qt4; }
QT_END_NAMESPACE

class qt4 : public QWidget

{
    Q_OBJECT

public:
    qt4(QWidget *parent = nullptr);
    ~qt4();
    void createFolder();

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            m_dragPosition = event->globalPosition().toPoint() - geometry().topLeft();
            event->accept();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (event->buttons() & Qt::LeftButton)
        {
            move(event->globalPosition().toPoint() - m_dragPosition);
            event->accept();
        }
    }



private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();


    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_toolButton_3_clicked();

    void on_toolButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_toolButton_5_clicked();

private:
    Ui::qt4 *ui;
    QString imagePath;//保存登录头像路径
    QString filename;//注册时头像路径
    QPoint m_dragPosition;


};

#endif // LOGIN_H
