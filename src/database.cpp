#include "database.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

Database::Database() : db(QSqlDatabase::addDatabase("QSQLITE", "fitCheckIn")) {}

Database::~Database() { db.close(); }

void Database::init()
{
    db.setDatabaseName("fit_check_in.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "错误", "数据库打开失败");
        qDebug() << db.lastError().text();
        return;
    } else {
        qDebug() << "数据库打开成功";
    }
    QSqlQuery query(db);
    query.exec("create table if not exists admin ("
               "pk_id integer primary key autoincrement,"
               "password varchar(20) unique not null default 'admin',"
               "password_reset_time datetime"
               ")");
    query.exec("select * from admin");
    if (!query.next()) {
        query.exec("insert into admin default values");
    }
    query.exec("create table if not exists user ("
               "pk_user_id integer primary key autoincrement,"
               "registration_date date default (date('now')),"
               "uk_name varchar(20) unique not null,"
               "password varchar(20) not null,"
               "uk_phone varchar(20),"
               "gender char(5)"
               ")");
    query.exec("create table if not exists sport ("
               "pk_sport_id integer primary key autoincrement,"
               "uk_name varchar(20) unique not null"
               ")");
    query.exec("create table if not exists check_in ("
               "pk_check_in_id integer primary key autoincrement,"
               "start_time datetime default (datetime('now')),"
               "user_id integer not null,"
               "sport_id integer not null,"
               "duration time default '00:00:00' not null,"
               "foreign key (user_id) references user (pk_user_id) on delete cascade,"
               "foreign key (sport_id) references sport (pk_sport_id)"
               ")");
    query.exec("select pk_sport_id from sport");
    if (!query.next()) {
        query.exec("insert into sport (uk_name) values"
                   "('跑步'),"
                   "('行走'),"
                   "('骑行'),"
                   "('球类'),"
                   "('游泳'),"
                   "('跳绳'),"
                   "('瑜伽'),"
                   "('器械'),"
                   "('舞蹈'),"
                   "('拳击'),"
                   "('击剑'),"
                   "('攀岩'),"
                   "('武术'),"
                   "('其它')");
    }
}
