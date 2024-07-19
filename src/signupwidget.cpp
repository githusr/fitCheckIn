#include "signupwidget.h"

#include "ui_signupwidget.h"

#include <QDate>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

SignupWidget::SignupWidget(QWidget *parent) : QWidget(parent), ui(new Ui::SignupWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

SignupWidget::~SignupWidget() { delete ui; }

void SignupWidget::on_pushButtonSignup_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("fitCheckIn");
    QSqlQuery query(db);
    QString userName = ui->lineEditUserName->text();
    QString password = ui->lineEditPassword->text();
    QString passwordAgain = ui->lineEditPasswordAgain->text();

    query.prepare("select uk_name from user where uk_name = ?");
    query.addBindValue(userName);
    query.exec();
    if (userName.isEmpty()) {
        QMessageBox::warning(this, "注册失败", "请输入用户名");
    } else if (query.next()) {
        QMessageBox::warning(this, "注册失败", "用户名已存在");
        ui->lineEditUserName->clear();
        ui->lineEditPassword->clear();
        ui->lineEditPasswordAgain->clear();
    } else if (password != passwordAgain) {
        QMessageBox::warning(this, "注册失败", "两次密码输入不一致");
        ui->lineEditPassword->clear();
        ui->lineEditPasswordAgain->clear();
    } else {
        query.prepare("insert into user (registration_date, uk_name, password) values "
                      "(:registration_date, :userName, :password)");
        query.bindValue(":registration_date", QDate::currentDate().toString("yyyy-MM-dd"));
        query.bindValue(":userName", userName);
        query.bindValue(":password", password);
        if (query.exec()) {
            QMessageBox::information(this, "注册成功", "您已成功注册。");
            ui->lineEditUserName->clear();
            ui->lineEditPassword->clear();
            ui->lineEditPasswordAgain->clear();
        } else {
            qDebug() << query.lastError();
            QMessageBox::critical(this, "注册失败", "注册过程中发生错误。");
        }
    }
}
