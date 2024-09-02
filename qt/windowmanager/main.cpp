#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QByteArray>
#include "windowmanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WindowManager manager;
    manager.setGeometry(0, 0, 800, 600);
    manager.setWindowTitle("My Window Manager");
    manager.show();

    return app.exec();
}
