#include"databasemanager.h"

DatabaseManager::DatabaseManager(const QString& hostname, int port, const QString& dbname, const QString& username, const QString& password) {
    m_hostname=hostname;
    m_port=port;
    m_dbname=dbname;
    m_username=username;
    m_password=password;
}

bool DatabaseManager::connect() {
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setHostName(m_hostname);
    m_db.setPort(m_port);
    m_db.setDatabaseName(m_dbname);
    m_db.setUserName(m_username);
    m_db.setPassword(m_password);
    if (!m_db.open()) {
        return false;
    }

    return true;
}

void DatabaseManager::disconnect() {
    m_db.close();
}


