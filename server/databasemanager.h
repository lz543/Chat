
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include<QSqlQuery>
#include <QDebug>


class DatabaseManager {
public:
    DatabaseManager(const QString& hostname, int port, const QString& dbname, const QString& username, const QString& password);
    bool connect();
    void disconnect();


private:
    QString m_hostname;
    int m_port;
    QString m_dbname;
    QString m_username;
    QString m_password;
    QSqlDatabase m_db;
};


#endif // DATABASEMANAGER_H
