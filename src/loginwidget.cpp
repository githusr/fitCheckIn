#include "loginwidget.h"

#include "ui_loginwidget.h"

#include "admindialog.h"
#include "adminmainwindow.h"
#include "database.h"
#include "signupwidget.h"
#include "usermainwindow.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LoginWidget), db(new Database)
{
    ui->setupUi(this);
    db->init();
}

LoginWidget::~LoginWidget()
{
    delete ui;
    delete db;
}

void LoginWidget::on_pushButtonLogin_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("fitCheckIn");
    QSqlQuery query(db);
    QString userName = ui->lineEditUserName->text();
    QString password = ui->lineEditPassword->text();

    query.prepare("select uk_name password from user where uk_name = :userName and password = :password");
    query.addBindValue(userName);
    query.addBindValue(password);
    query.exec();

    if (query.next()) {
        QMessageBox::information(this, "登录成功", "您已成功登录。");
        hide();
        showUserWindow(userName);
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误。");
    }
    ui->lineEditPassword->clear();
}

void LoginWidget::on_pushButtonSignup_clicked()
{
    auto signupWidget = new SignupWidget;
    hide();
    signupWidget->show();
    connect(signupWidget, &QObject::destroyed, this, &QWidget::show);
}

void LoginWidget::on_pushButtonAdmin_clicked()
{
    auto adminDialog = new AdminDialog;
    hide();
    adminDialog->show();
    connect(adminDialog, &QObject::destroyed, this, &QWidget::show);
    connect(adminDialog, &AdminDialog::loginSuccess, this, [=] {
        auto adminMainWindow = new AdminMainWindow;
        adminMainWindow->show();
        disconnect(adminDialog, &QObject::destroyed, this, &QWidget::show);
        connect(adminMainWindow, &QObject::destroyed, this, &QWidget::show);
    });
}

void LoginWidget::showUserWindow(QString userName)
{
    auto userMainWindow = new UserMainWindow(userName);
    userMainWindow->show();
    connect(userMainWindow, &QObject::destroyed, this, &QWidget::show);
}
