#include <QApplication>
#include <QScreen>
#include "windowmanager.h"
#include "taskbar.h"
#include <QFile>
#include <QTimer>

void checkForSignalFile() {
    QFile file("/usr/cydra/temp/centralSign");
    if (file.exists()) {
        TaskBar taskBar;
        if(TaskBar.isPopupVisible) {
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
    QObject::connect(&timer, &QTimer::timeout, checkForSignalFile);
    timer.start(500); 
    
    taskBar.show();
    return app.exec();
}
