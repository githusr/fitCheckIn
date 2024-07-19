#include "adminmainwindow.h"

#include "ui_adminmainwindow.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>

AdminMainWindow::AdminMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AdminMainWindow), db(QSqlDatabase::database("fitCheckIn"))
{
    ui->setupUi(this);
    setCentralWidget(ui->stackedWidget);
    ui->stackedWidget->setCurrentIndex(0);
    setAttribute(Qt::WA_DeleteOnClose);

    setUserManagement();
    setSportManagement();
    setPasswordManagement();
}

AdminMainWindow::~AdminMainWindow() { delete ui; }

void AdminMainWindow::on_actionUser_triggered() { ui->stackedWidget->setCurrentIndex(0); }

void AdminMainWindow::on_actionSport_triggered() { ui->stackedWidget->setCurrentIndex(1); }

void AdminMainWindow::on_actionResetPassword_triggered() { ui->stackedWidget->setCurrentIndex(2); }

void AdminMainWindow::setUserManagement()
{
    ui->tableViewUser->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewUser->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewUser->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewUser->setAlternatingRowColors(true);

    auto sqlTableModel = new QSqlTableModel(this, db);
    sqlTableModel->setTable("user");
    sqlTableModel->select();
    sqlTableModel->setHeaderData(sqlTableModel->fieldIndex("pk_user_id"), Qt::Horizontal, "ID");
    sqlTableModel->setHeaderData(sqlTableModel->fieldIndex("registration_date"), Qt::Horizontal, "注册日期");
    sqlTableModel->setHeaderData(sqlTableModel->fieldIndex("uk_name"), Qt::Horizontal, "用户名");
    sqlTableModel->setHeaderData(sqlTableModel->fieldIndex("gender"), Qt::Horizontal, "性别");
    sqlTableModel->setHeaderData(sqlTableModel->fieldIndex("uk_phone"), Qt::Horizontal, "手机号");

    ui->tableViewUser->setModel(sqlTableModel);
    ui->tableViewUser->hideColumn(sqlTableModel->fieldIndex("password"));

    connect(ui->radioButtonAscending, &QRadioButton::clicked, this, [=] {
        int index;
        if (ui->comboBoxField->currentIndex() == 0) {
            index = sqlTableModel->fieldIndex("pk_user_id");
        } else {
            index = sqlTableModel->fieldIndex("uk_name");
        }
        sqlTableModel->sort(index, Qt::AscendingOrder);
    });
    connect(ui->radioButtonDescending, &QRadioButton::clicked, this, [=] {
        int index;
        if (ui->comboBoxField->currentIndex() == 0) {
            index = sqlTableModel->fieldIndex("pk_user_id");
        } else {
            index = sqlTableModel->fieldIndex("uk_name");
        }
        sqlTableModel->sort(index, Qt::DescendingOrder);
    });
    connect(ui->comboBoxField, &QComboBox::currentIndexChanged, this, [=](int index) {
        if (index) {
            index = 2;
        }
        if (ui->radioButtonAscending->isChecked()) {
            sqlTableModel->sort(index, Qt::AscendingOrder);
        } else {
            sqlTableModel->sort(index, Qt::DescendingOrder);
        }
    });
    connect(ui->lineEditUserName, &QLineEdit::textChanged, this,
            [=](QString str) { sqlTableModel->setFilter(QString("uk_name like '%%1%'").arg(str)); });
    connect(ui->pushButtonDeleteUser, &QPushButton::clicked, this, [=] {
        sqlTableModel->removeRow(ui->tableViewUser->currentIndex().row());
        sqlTableModel->select();
    });
}

void AdminMainWindow::setSportManagement()
{
    auto sqlTableModel = new QSqlTableModel(this, db);
    sqlTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    sqlTableModel->setTable("sport");
    sqlTableModel->setHeaderData(sqlTableModel->fieldIndex("uk_name"), Qt::Horizontal, "运动项目");
    sqlTableModel->select();

    ui->tableViewSport->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewSport->setModel(sqlTableModel);
    ui->tableViewSport->hideColumn(sqlTableModel->fieldIndex("pk_sport_id"));

    auto itemSelectionModel = new QItemSelectionModel(sqlTableModel, this);
    ui->tableViewSport->setSelectionModel(itemSelectionModel);

    connect(ui->pushButtonAppend, &QPushButton::clicked, this, [=] {
        sqlTableModel->insertRecord(sqlTableModel->rowCount(), sqlTableModel->record());
        itemSelectionModel->clearSelection();
        itemSelectionModel->setCurrentIndex(sqlTableModel->index(sqlTableModel->rowCount() - 1, 1),
                                            QItemSelectionModel::Select);
    });
    connect(ui->pushButtonInsert, &QPushButton::clicked, this, [=] {
        auto currentIndex = ui->tableViewSport->currentIndex();
        sqlTableModel->insertRecord(currentIndex.row(), sqlTableModel->record());
        itemSelectionModel->clearSelection();
        itemSelectionModel->setCurrentIndex(currentIndex, QItemSelectionModel::Select);
    });
    connect(ui->pushButtonDelete, &QPushButton::clicked, this,
            [=] { sqlTableModel->removeRow(ui->tableViewSport->currentIndex().row()); });
    connect(ui->pushButtonSave, &QPushButton::clicked, this, [=] {
        sqlTableModel->submitAll();
        sqlTableModel->select();
    });
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, [=] {
        sqlTableModel->revertAll();
        sqlTableModel->select();
    });
}

void AdminMainWindow::setPasswordManagement()
{
    QSqlQuery query(db);
    query.exec("select password_reset_time from admin");
    query.next();
    if (query.value(0).isNull()) {
        ui->labelModify->hide();
        ui->dateTimeEdit->hide();
    } else {
        ui->dateTimeEdit->setDateTime(
            QDateTime::fromString(query.value(0).toString(), "yyyy-MM-dd HH:mm:ss"));
    }
    connect(ui->pushButtonOk, &QPushButton::clicked, this, [=] {
        QSqlQuery query(db);
        query.prepare("update admin set password = :newPassword, password_reset_time = :passwordResetTime "
                      "where pk_id=1");
        query.bindValue(":newPassword", ui->lineEditPassword->text());
        query.bindValue(":passwordResetTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        if (query.exec()) {
            QMessageBox::information(this, "修改成功", "密码已成功修改");
            ui->lineEditPassword->clear();
            query.exec("select password_reset_time from admin");
            query.next();
            ui->dateTimeEdit->setDateTime(
                QDateTime::fromString(query.value(0).toString(), "yyyy-MM-dd HH:mm:ss"));
            ui->labelModify->show();
            ui->dateTimeEdit->show();
        }
    });
}
