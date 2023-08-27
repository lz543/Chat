
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>


class DatabaseManager {
public:

    DatabaseManager(const QString& dbPath,const QString& connectionName);

    ~DatabaseManager();


    bool openDatabase() ;

    bool createTabel();

    void closeDatabase() ;

    QSqlDatabase getDatabase();

    bool insertMessage(const int& self,const int& friendId,const QString& friendName,const QString& content,const QString& time,const int& isVerifyMessage) ;

private:
    QSqlDatabase database;
    QString connectionName;

};

#endif // DATABASEMANAGER_H
