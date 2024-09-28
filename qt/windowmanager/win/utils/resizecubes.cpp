#include "resizecubes.h"
#include <QRect>

ResizeCubes::ResizeCubes(QWidget *parent)
    : parentWidget(parent) {
}

void ResizeCubes::createTrackingSquares(WId windowId, const QRect &geometry) {
    TrackingSquares squares;

    squares.leftSquare = new QLabel(parentWidget);
    squares.leftSquare->setGeometry(geometry.left() - 5, geometry.top(), 5, geometry.height());
    squares.leftSquare->setStyleSheet("background-color: rgba(255, 0, 0, 0.5)");

    squares.rightSquare = new QLabel(parentWidget);
    squares.rightSquare->setGeometry(geometry.right(), geometry.top(), 5, geometry.height());
    squares.rightSquare->setStyleSheet("background-color: rgba(0, 255, 0, 0.5)");

    squares.bottomSquare = new QLabel(parentWidget);
    squares.bottomSquare->setGeometry(geometry.left(), geometry.bottom(), geometry.width(), 5);
    squares.bottomSquare->setStyleSheet("background-color: rgba(0, 0, 255, 0.5)");

    squares.leftSquare->show();
    squares.rightSquare->show();
    squares.bottomSquare->show();

    windowSquares[windowId] = squares;
    windowSquares.insert(windowId, {leftSquare, rightSquare, bottomSquare});
}

void ResizeCubes::updateTrackingSquares(WId windowId, const QRect &geometry) {
    if (windowSquares.contains(windowId)) {
        TrackingSquares &squares = windowSquares[windowId];

        squares.leftSquare->setGeometry(geometry.left() - 5, geometry.top(), 5, geometry.height());
        squares.rightSquare->setGeometry(geometry.right(), geometry.top(), 5, geometry.height());
        squares.bottomSquare->setGeometry(geometry.left(), geometry.bottom(), geometry.width(), 5);
    }
}

void ResizeCubes::killTrackingCubes() {
    for (auto &squares : windowSquares) {
        delete squares.leftSquare;
        delete squares.rightSquare;
        delete squares.bottomSquare;
    }
    windowSquares.clear();
}

QRect ResizeCubes::getWindowGeometry(WId windowId) const {
    if (windowManager && windowManager->trackedWindows.contains(windowId)) {
        return windowManager->trackedWindows.value(windowId)->geometry();
    }

    return QRect();
}
