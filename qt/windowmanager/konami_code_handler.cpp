#include "konami_code_handler.h"
#include <QDebug>

const int KonamiCodeHandler::konamiCode[] = {
    Qt::Key_Up, Qt::Key_Up, Qt::Key_Down, Qt::Key_Down,
    Qt::Key_Left, Qt::Key_Right, Qt::Key_Left, Qt::Key_Right,
    Qt::Key_B, Qt::Key_A
};

const int KonamiCodeHandler::konamiCodeLength = sizeof(konamiCode) / sizeof(konamiCode[0]);

KonamiCodeHandler::KonamiCodeHandler(QObject *parent)
    : QObject(parent), currentCodeIndex(0), consoleOpened(false), consoleProcess(nullptr) {}

void KonamiCodeHandler::keyPressEvent(QKeyEvent *event) {
    if (event->key() == konamiCode[currentCodeIndex]) {
        ++currentCodeIndex;
        if (currentCodeIndex == konamiCodeLength) {
            qDebug() << "Konami code entered. Opening console.";
            openConsole();
            currentCodeIndex = 0;
        }
    } else {
        currentCodeIndex = 0;
    }

    if (consoleOpened && event->key() == Qt::Key_Escape) {
        qDebug() << "ESC pressed. Closing console.";
        closeConsole();
    }
}

void KonamiCodeHandler::openConsole() {
    if (!consoleOpened && !consoleProcess) {
        consoleProcess = new QProcess(this);
        consoleProcess->start("xterm");
        consoleOpened = true;
    }
}

void KonamiCodeHandler::closeConsole() {
    if (consoleProcess) {
        consoleProcess->terminate();
        consoleProcess->waitForFinished();
        delete consoleProcess;
        consoleProcess = nullptr;
        consoleOpened = false;
    }
}
