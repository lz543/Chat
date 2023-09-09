
#include "mytcpserver.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServer *s=new TcpServer();
    s->start("127.0.0.1",9999);
    return a.exec();
}

