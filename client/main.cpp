
#include "login.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qt4 *w=new qt4();
    w->show();
    qt4 *w1=new qt4();
    w1->show();
    return a.exec();
}
