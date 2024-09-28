#ifndef RESIZECUBES_H
#define RESIZECUBES_H

#include <QMap>
#include <QLabel>
#include <QRect>
#include <X11/Xlib.h>
#include <QWidget>

class ResizeCubes {
public:
    explicit ResizeCubes(QWidget *parent = nullptr);
    void createTrackingSquares(WId windowId, const QRect &geometry);
    void updateTrackingSquares(WId windowId, const QRect &geometry);
    void killTrackingCubes();
    TrackingSquares getTrackingSquares(WId windowId) const;
    QRect getWindowGeometry(WId windowId) const;

    struct TrackingSquares {
        QLabel *leftSquare;
        QLabel *rightSquare;
        QLabel *bottomSquare;
    };

private:
    QMap<WId, TrackingSquares> windowSquares;
    QWidget *parentWidget;
    WindowManager* windowManager;
};

#endif // RESIZECUBES_H
