#include "konami_code_handler.h"
#include <QDebug>
#include <QLabel>

const int KonamiCodeHandler::konamiCode[] = {
    Qt::Key_Up, Qt::Key_Up, Qt::Key_Down, Qt::Key_Down,
    Qt::Key_Left, Qt::Key_Right, Qt::Key_Left, Qt::Key_Right,
    Qt::Key_B, Qt::Key_A
};

const int KonamiCodeHandler::konamiCodeLength = sizeof(konamiCode) / sizeof(konamiCode[0]);

KonamiCodeHandler::KonamiCodeHandler(QLabel *logLabel, QObject *parent)
    : QObject(parent), logLabel(logLabel), currentCodeIndex(0) {}

void KonamiCodeHandler::keyPressEvent(QKeyEvent *event) {
    if (event->key() == konamiCode[currentCodeIndex]) {
        ++currentCodeIndex;
        if (currentCodeIndex == konamiCodeLength) {
            appendLog("Konami code entered. Displaying system warnings.");
            currentCodeIndex = 0;
        }
    } else {
        currentCodeIndex = 0;
    }

    if (event->key() == Qt::Key_Escape) {
        appendLog("ESC pressed. Clearing warnings.");
        logLabel->clear();
    }
}

void KonamiCodeHandler::appendLog(const QString &message) {
    if (logLabel) {
        QString currentText = logLabel->text();
        logLabel->setText(currentText + "\n" + message);
    }
}
