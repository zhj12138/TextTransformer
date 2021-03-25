#include "myTrayIcon.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto trayIcon = new MyTrayIcon();

    return app.exec();
}
