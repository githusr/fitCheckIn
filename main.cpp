#include "loginwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LoginWidget window;
    window.show();
    return app.exec();
}
