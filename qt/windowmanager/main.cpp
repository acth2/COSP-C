#include <QApplication>
#include <QScreen>
#include "windowmanager.h"
#include "taskbar.h"
#include <QFile>
#include <QTimer>

void checkForSignalFile(TaskBar &taskBar) {
    QFile file("/usr/cydra/temp/centralSign");
    if (file.exists()) {
        if (taskBar.isPopupVisible()) {
            taskBar.closePopup();
        } else {
            taskBar.showPopup();
        }
        file.remove();
    }
}

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

    QTimer timer;

    QObject::connect(&timer, &QTimer::timeout, [&taskBar]() { checkForSignalFile(taskBar); });
    timer.start(500);

    taskBar.show();
    return app.exec();
}
