#pragma once

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTime>

namespace Ui
{
    class UserMainWindow;
}

class UserMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserMainWindow(QString userName, QWidget *parent = nullptr);
    ~UserMainWindow();

private slots:
    void on_actionAbout_triggered();
    void on_actionHome_triggered();
    void on_actionSport_triggered();
    void on_actionRecord_triggered();
    void on_actionProfile_triggered();

private:
    void setSportPage();
    void setRecordPage();
    void setProfilePage();

private:
    int userID;

private:
    Ui::UserMainWindow *ui;
    QSqlDatabase db;

private:
    QTime duration;
    QDateTime startTime;
};
