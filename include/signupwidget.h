#pragma once

#include <QWidget>

namespace Ui
{
    class SignupWidget;
}

class SignupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SignupWidget(QWidget *parent = nullptr);
    ~SignupWidget();

private slots:
    void on_pushButtonSignup_clicked();

private:
    Ui::SignupWidget *ui;
};
