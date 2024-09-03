#include <QApplication>
#include "userinteractright.h"
#include <QScreen>
#include "windowmanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WindowManager manager;

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        manager.setGeometry(screenGeometry);
    }
    
    UserInteractRight interactWidget;
    interactWidget.show();
    manager.setWindowTitle("CWM");
    manager.showFullScreen();

    return app.exec();
}
