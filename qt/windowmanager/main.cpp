#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QByteArray>
#include "windowmanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Vérifiez si la variable d'environnement DISPLAY est définie en utilisant QProcess
    QProcess process;
    process.start("sh", QStringList() << "-c" << "echo $DISPLAY");
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput().trimmed();

    if (!output.isEmpty()) {
        WindowManager manager;
        manager.setGeometry(0, 0, 800, 600);
        manager.setWindowTitle("My Window Manager");
        manager.show();

        return app.exec();
    } else {
        qDebug() << "Erreur : La variable d'environnement DISPLAY n'est pas définie. Assurez-vous d'avoir lancé un serveur X";
        return 1;
    }
}
