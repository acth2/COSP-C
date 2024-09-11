#include <QApplication>
#include <QScreen>
#include "windowmanager.h"
#include "taskbar.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WindowManager manager;
    TaskBar taskBar;

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        manager.setGeometry(screenGeometry);
    }

    manager.setWindowTitle("CWM");
    manager.showFullScreen();

    taskBar.show();
    return app.exec();
}
