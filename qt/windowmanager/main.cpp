#include "windowmanager.h"
#include "taskbar.h"
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtMessageHandler>
#include <QMessageBox>
#include <QGuiApplication>

QFile logFile;

void customLogOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ")
        << msg << endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    logFile.setFileName("/usr/cydra/logs/a2wm.log");
    logFile.open(QIODevice::Append | QIODevice::Text);

    qInstallMessageHandler(customLogOutput);

    QString platform = QGuiApplication::platformName();

    if (platform != "xcb") {
        QMessageBox::critical(nullptr, "Error", "Sorry ! The windowmanager A2WM only works with X11, try rebooting it using the command startx $(which a2wm) \n (if the command which is installed on your computer else you will need to provide the path manually)");
        return -1;
    }
    
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
