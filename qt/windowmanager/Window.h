#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QX11Info>
#include <QProcess>
#include <QPushButton>
#include <QWidget>
#include <X11/Xlib.h>

class Window : public QMainWindow {
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    void monitorWindows();

private:
    void setupUI(WId targetWinId);
    void moveWindow(WId targetWinId);
    void resizeWindow(WId targetWinId);

    QWidget *topBar;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;

    QProcess *process;

    WId currentWinId;
    bool isFullScreenMode;
    bool dragging;
    bool resizing;
    QPoint dragStartPosition;
    QSize resizeStartSize;
};

#endif // WINDOW_H
