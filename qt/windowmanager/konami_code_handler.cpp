#include "konami_code_handler.h"

KonamiCodeHandler::KonamiCodeHandler(QObject *parent) : QObject(parent), currentSequence("") {}

void KonamiCodeHandler::handleKeyPress(QKeyEvent *event) {
    currentSequence += QString::number(event->key());

    if (currentSequence.length() > konamiCode.length()) {
        currentSequence = currentSequence.right(konamiCode.length());
    }

    if (currentSequence == konamiCode) {
        emit konamiCodeEntered();
        currentSequence.clear();
    }
}
