#pragma once

#include <QSqlDatabase>

class Database
{
public:
    Database();
    ~Database();
    void init();

private:
    QSqlDatabase db;
};
