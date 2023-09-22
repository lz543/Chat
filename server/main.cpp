
#include "mytcpserver.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServer s;
    s.start("192.168.0.104",8888);
    return a.exec();
}

