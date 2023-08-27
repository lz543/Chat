#include "databasemanager.h"


DatabaseManager::DatabaseManager(const QString& dbPath,const QString &connectionName) {
    this->connectionName=connectionName;
    if (QSqlDatabase::contains(connectionName)) {

        database = QSqlDatabase::database(connectionName);
    }
    else {

        database = QSqlDatabase::addDatabase("QSQLITE",connectionName);
        database.setDatabaseName(dbPath);
    }
    if (!database.open()) {
        qDebug() << "无法建立数据库连接";

    }
    else{
        QSqlQuery query(database);
        if (!query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='message';")) {
            qDebug() << "数据库查询失败";
        }else{
            if (!query.next()) {
                // 表格不存在，创建表格
                QString creat_sql = "CREATE TABLE message(id INTEGER PRIMARY KEY, self INTEGER, friendId INTEGER, friendName TEXT, content TEXT, time DATETIME, isDelete INTEGER DEFAULT 0, isVerifyMessage INTEGER)";
                if (!query.exec(creat_sql)) {
                    qDebug() << "无法创建表格";

                }else{
                    qDebug()<<"创建成功";
                }
            }
        }


        database.close();
    }

}


DatabaseManager::~DatabaseManager() {
    closeDatabase();

}

bool DatabaseManager::openDatabase() {
    if(database.open()){
        return true;
    } else {
        qDebug() << "Failed to open database";
        return false;
    }

}

QSqlDatabase  DatabaseManager::getDatabase() {
    return this->database;

}

void DatabaseManager::closeDatabase() {
    database.close();

}


bool DatabaseManager::insertMessage(const int& self, const int& friendId,const QString& friendName,const QString& content,const QString& time,const int& isVerifyMessage) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO message (self, friendId, friendName, content, time,isVerifyMessage) "
                  "VALUES (:self, :friendId, :friendName, :content, :time ,:isVerifyMessage)");
    query.bindValue(":self", self);  //0或1
    query.bindValue(":friendId", friendId);
    query.bindValue(":friendName", friendName);
    query.bindValue(":content", content);
    query.bindValue(":time", time);
    query.bindValue(":isVerifyMessage", isVerifyMessage);
    return query.exec();
}



