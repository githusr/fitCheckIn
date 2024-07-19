#include "usermainwindow.h"

#include "ui_usermainwindow.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QString>
#include <QTimer>

UserMainWindow::UserMainWindow(QString userName, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::UserMainWindow), db(QSqlDatabase::database("fitCheckIn"))
{
    ui->setupUi(this);
    setCentralWidget(ui->stackedWidget);
    ui->stackedWidget->setCurrentIndex(0);
    ui->statusbar->showMessage(QString("欢迎您，%1").arg(userName));
    setAttribute(Qt::WA_DeleteOnClose);

    QSqlQuery query(db);
    query.prepare("select pk_user_id from user where uk_name = :userName");
    query.bindValue(":userName", userName);
    query.exec();
    query.next();
    userID = query.value("pk_user_id").toInt();

    setSportPage();
    setRecordPage();
    setProfilePage();
}

UserMainWindow::~UserMainWindow() { delete ui; }

void UserMainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "关于",
                       "运动健身打卡管理系统\n"
                       "本应用旨在帮助用户跟踪和管理健身活动\n"
                       "系统使用开源Qt框架开发\n\n"
                       "感谢您的使用");
}

void UserMainWindow::on_actionHome_triggered() { ui->stackedWidget->setCurrentIndex(0); }

void UserMainWindow::on_actionSport_triggered() { ui->stackedWidget->setCurrentIndex(1); }

void UserMainWindow::on_actionRecord_triggered() { ui->stackedWidget->setCurrentIndex(2); }

void UserMainWindow::on_actionProfile_triggered() { ui->stackedWidget->setCurrentIndex(3); }

void UserMainWindow::setSportPage()
{
    ui->pushButtonSuspend->hide();
    ui->pushButtonEnd->hide();
    ui->pushButtonContinue->hide();

    QSqlQuery query(db);
    query.exec("select uk_name from sport");
    while (query.next()) {
        if (query.value(0).toString() != "其它") {
            ui->comboBoxSport->addItem(query.value(0).toString());
        }
    }
    ui->comboBoxSport->addItem("其它");

    duration = QTime(0, 0);
    ui->lcdNumber->display("00:00:00");

    auto timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [=] {
        duration = duration.addSecs(1);
        ui->lcdNumber->display(duration.toString("hh:mm:ss"));
    });

    connect(ui->pushButtonBegin, &QPushButton::clicked, this, [=] {
        startTime = QDateTime::currentDateTime();
        timer->start(1000);
        ui->pushButtonBegin->hide();
        ui->pushButtonSuspend->show();
        ui->comboBoxSport->setEnabled(false);
    });

    connect(ui->pushButtonSuspend, &QPushButton::clicked, this, [=] {
        timer->stop();
        ui->pushButtonSuspend->hide();
        ui->pushButtonEnd->show();
        ui->pushButtonContinue->show();
    });

    connect(ui->pushButtonContinue, &QPushButton::clicked, this, [=] {
        timer->start();
        ui->pushButtonEnd->hide();
        ui->pushButtonContinue->hide();
        ui->pushButtonSuspend->show();
    });

    connect(ui->pushButtonEnd, &QPushButton::clicked, this, [=]() {
        timer->stop();
        ui->lcdNumber->display("00:00:00");

        ui->pushButtonEnd->hide();
        ui->pushButtonContinue->hide();
        ui->pushButtonBegin->show();

        QSqlQuery query(db);
        QSqlQuery query_2(db);
        query.prepare("insert into check_in (start_time, user_id, sport_id, duration) values (:startTime, "
                      ":userID, :sportID, :duration)");
        query.bindValue(":startTime", startTime.toString("yyyy-MM-dd HH:mm:ss"));
        query.bindValue(":userID", userID);
        query.bindValue(":duration", duration.toString("HH:mm:ss"));

        query_2.prepare("select pk_sport_id from sport where uk_name = :sport");
        query_2.bindValue(":sport", ui->comboBoxSport->currentText());
        query_2.exec();
        query_2.next();

        query.bindValue(":sportID", query_2.value(0).toInt());

        if (query.exec()) {
            QMessageBox::information(this, "运动结束", "打卡成功！");
        }

        duration = QTime(0, 0);
        ui->comboBoxSport->setEnabled(true);
    });
}

void UserMainWindow::setRecordPage()
{
    auto sqlQueryModel = new QSqlQueryModel(this);
    sqlQueryModel->setQuery(
        QString("select start_time 开始时间, duration 运动总时长, uk_name 运动项目 from check_in inner join "
                "sport on check_in.sport_id=sport.pk_sport_id where user_id=%1")
            .arg(userID),
        db);
    ui->tableView->setModel(sqlQueryModel);
    connect(ui->actionRecord, &QAction::triggered, this, [=] {
        sqlQueryModel->setQuery(
            QString(
                "select start_time 开始时间, duration 运动总时长, uk_name 运动项目 from check_in inner join "
                "sport on check_in.sport_id=sport.pk_sport_id where user_id=%1")
                .arg(userID),
            db);
        ui->tableView->resizeColumnsToContents();
    });
}

void UserMainWindow::setProfilePage()
{
    ui->lineEditPhone->setValidator(
        new QRegularExpressionValidator(QRegularExpression("^1[3-9]\\d{9}$"), ui->lineEditPhone));

    QSqlQuery query(db);
    query.prepare("select registration_date, uk_name, uk_phone, gender from user where "
                  "pk_user_id = :userID");
    query.bindValue(":userID", userID);
    query.exec();
    query.next();

    auto registrationDate = query.value(0);
    auto userName = query.value(1);
    auto phone = query.value(2);
    auto gender = query.value(3);

    ui->dateEdit->setDate(QDate::fromString(registrationDate.toString(), "yyyy-MM-dd"));

    ui->lineEditUserName->setText(userName.toString());

    ui->lineEditPhone->setText(phone.toString());

    ui->comboBoxGender->setCurrentText(gender.toString());

    connect(ui->pushButtonSave, &QPushButton::clicked, this, [=] {
        QSqlQuery query(db);
        query.prepare("update user set uk_phone = :phone, gender = :gender where "
                      "pk_user_id = :userID");
        query.bindValue(":phone", ui->lineEditPhone->text());
        query.bindValue(":gender", ui->comboBoxGender->currentText());
        query.bindValue(":userID", userID);

        if (query.exec()) {
            QMessageBox::information(this, "保存成功", "个人信息修改成功");
        } else {
            qDebug() << query.lastError();
        }
    });

    connect(ui->pushButtonRestore, &QPushButton::clicked, this, [=] {
        QSqlQuery query(db);
        query.prepare("select gender, uk_phone from user where pk_user_id=:userID");
        query.addBindValue(userID);
        query.exec();
        query.next();
        ui->comboBoxGender->setCurrentText(query.value(0).toString());
        ui->lineEditPhone->setText(query.value(1).toString());
    });

    connect(ui->pushButtonResetPassword, &QPushButton::clicked, this,
            [=] { ui->stackedWidget->setCurrentIndex(4); });

    connect(ui->pushButtonOk, &QPushButton::clicked, this, [=] {
        QSqlQuery query(db);
        query.prepare("update user set password = :newPassword where pk_user_id = :userID");
        query.bindValue(":newPassword", ui->lineEditPassword->text());
        query.bindValue(":userID", userID);
        if (query.exec()) {
            QMessageBox::information(this, "修改成功", "密码已成功修改");
            ui->stackedWidget->setCurrentIndex(3);
        }
    });
}
