#include "admindialog.h"

#include "ui_admindialog.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTimer>

AdminDialog::AdminDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AdminDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->labelError->hide();
}

AdminDialog::~AdminDialog() { delete ui; }

void AdminDialog::on_pushButtonOk_clicked()
{
    QSqlDatabase db = QSqlDatabase::database("fitCheckIn");
    QSqlQuery query(db);
    query.exec("select password from admin");
    query.next();
    QString password = query.value("password").toString();

    if (ui->lineEditPassword->text() != password) {
        ui->labelError->show();
        QTimer::singleShot(500, this, [=] { ui->labelError->hide(); });
        ui->lineEditPassword->clear();
    } else {
        emit loginSuccess();
        close();
    }
}
