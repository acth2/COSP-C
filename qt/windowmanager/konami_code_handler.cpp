#include "konami_code_handler.h"
#include <QDebug>

KonamiCodeHandler::KonamiCodeHandler(QObject *parent) : QObject(parent) {}

void KonamiCodeHandler::handleKeyPress(QKeyEvent *event) {
    QString keyStr = QString::number(event->key());
    currentSequence.append(keyStr);

    if (currentSequence.length() > konamiCode.length()) {
        currentSequence.remove(0, currentSequence.length() - konamiCode.length());
    }

    if (currentSequence == konamiCode) {
        emit konamiCodeEntered();
        currentSequence.clear();
    }
}
