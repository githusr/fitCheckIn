#pragma once

#include "database.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class LoginWidget;
}
QT_END_NAMESPACE

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private slots:
    void on_pushButtonLogin_clicked();
    void on_pushButtonSignup_clicked();
    void on_pushButtonAdmin_clicked();

private:
    void showUserWindow(QString userName);

private:
    Ui::LoginWidget *ui;
    Database *db;
};
