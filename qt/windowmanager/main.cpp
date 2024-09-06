#include <QApplication>
#include <QScreen>
#include "windowmanager.h"
#include "taskbar.h"
#include "global_key_hook.cpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WindowManager manager;
    TaskBar taskBar;

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        manager.setGeometry(screenGeometry);
    }
    
    TaskBar taskbar;
    GlobalKeyListener listener(&taskbar);

    manager.setWindowTitle("CWM");
    manager.showFullScreen();

    listener.startListening();
    taskBar.show();
    return app.exec();
}
