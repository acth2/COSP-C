#include <QApplication>
#include <QScreen>
#include <QKeyEvent>
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

void TaskBar::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape && popup->isVisible()) {
        closePopup();
    }
    
    if (event->key() == Qt::Key_Meta || event->key() == Qt::Key_Super_L || event->key() == Qt::Key_Super_R) {
        showPopup = !showPopup;
    }
    QWidget::keyPressEvent(event);
}
