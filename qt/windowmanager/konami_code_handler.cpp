#include "konami_code_handler.h"

KonamiCodeHandler::KonamiCodeHandler(QObject *parent)
    : QObject(parent), resetTimer(new QTimer(this)) {
    
    resetTimer->setInterval(2000);
    resetTimer->setSingleShot(true);

    connect(resetTimer, &QTimer::timeout, this, &KonamiCodeHandler::resetSequence);
}

void KonamiCodeHandler::handleKeyPress(QKeyEvent *event) {
    resetTimer->start();

    if (!konamiCode.isEmpty() && event->key() == konamiCode[currentSequence.size()]) {
        currentSequence.append(event->key());

        if (currentSequence == konamiCode) {
            emit konamiCodeEntered();
            resetSequence();
        }
    } else {
        resetSequence();
    }
}

void KonamiCodeHandler::resetSequence() {
    currentSequence.clear();
    resetTimer->stop();
}
