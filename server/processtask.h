
#ifndef PROCESSTASK_H
#define PROCESSTASK_H

#include<QtNetwork>


class processTask
{
public:
    processTask(QByteArray jsonData) :jsonData(jsonData) {}


private:
    QByteArray jsonData;
};

#endif // PROCESSTASK_H
