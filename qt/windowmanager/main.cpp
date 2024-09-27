#include "windowmanager.h"
#include "taskbar.h"
#include <signal.h>
#include <iostream>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtMessageHandler>

QFile logFile;

void customLogOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
        << msg << endl;
}

void signal_handler(int signal) {
    if (signal == SIGTERM || signal == SIGINT) {
        std::cout << "INFO: SIGTERM / SIGINT SIGNAL DETECTED" << std::endl;
    } else if (signal == SIGSEGV) {
        std::cerr << "CRITICAL ERROR : Segmentation Fault" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGSEGV, signal_handler);

    logFile.setFileName("/usr/cydra/logs/a2wm.log");
    logFile.open(QIODevice::Append | QIODevice::Text);

    qInstallMessageHandler(customLogOutput);
    
    WindowManager manager;
    TaskBar taskBar;

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        manager.setGeometry(screenGeometry);
    }

    manager.setWindowTitle("A2WM");
    manager.showFullScreen();

    taskBar.show();
    return app.exec();
}
