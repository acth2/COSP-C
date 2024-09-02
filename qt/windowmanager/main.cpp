#include <QApplication>
#include <QScreen>
#include "windowmanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WindowManager manager;

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        manager.setGeometry(screenGeometry);
        qDebug() << "Screen size:" << screenGeometry.size();
    } else {
        qDebug() << "Erreur : Impossible d'obtenir les informations sur l'écran.";
    }

    manager.setWindowTitle("CWM");
    manager.showFullScreen();

    return app.exec();
}
