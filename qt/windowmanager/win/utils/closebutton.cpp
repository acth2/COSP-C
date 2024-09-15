#include "closebutton.h"
#include <QApplication>

CloseButton::CloseButton(QWindow *trackedWindow, QWidget *parent)
    : QPushButton(parent), trackedWindow(trackedWindow) {

    setText("✕");
    setFixedSize(30, 27);

    connect(this, &QPushButton::clicked, this, &CloseButton::handleButtonClicked);
}

void CloseButton::updatePosition() {
    if (trackedWindow) {
        QRect windowGeometry = trackedWindow->geometry();
        setGeometry(windowGeometry.right() - 40, windowGeometry.top(), 30, 30);
        show();
    }
}

void CloseButton::handleButtonClicked() {
    emit closeRequested();
}
