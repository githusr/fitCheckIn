#pragma once

#include <QMainWindow>
#include <QSqlDatabase>

namespace Ui
{
    class AdminMainWindow;
}

class AdminMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminMainWindow(QWidget *parent = nullptr);
    ~AdminMainWindow();

private slots:
    void on_actionUser_triggered();
    void on_actionSport_triggered();
    void on_actionResetPassword_triggered();

private:
    void setUserManagement();
    void setSportManagement();
    void setPasswordManagement();

private:
    Ui::AdminMainWindow *ui;
    QSqlDatabase db;
};
