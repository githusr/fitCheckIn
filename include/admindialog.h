#pragma once

#include <QDialog>

namespace Ui
{
    class AdminDialog;
}

class AdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminDialog(QWidget *parent = nullptr);
    ~AdminDialog();

signals:
    void loginSuccess();

private slots:
    void on_pushButtonOk_clicked();

private:
    Ui::AdminDialog *ui;
};
