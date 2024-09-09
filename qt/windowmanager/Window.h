#ifndef WINDOW_H
#define WINDOW_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <QObject>
#include <QWidget>
#include <QProcess>

class Window : public QWidget {
    Q_OBJECT

public:
    explicit Window(Display *display, Window rootWindow, QWidget *parent = nullptr);
    ~Window();

    void detectWindows();
    void attachToWindow(Window targetWindow);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Display *display;
    Window rootWindow;

    Window targetWindow;
    bool dragging;
    QPoint dragStartPosition;

    void addTopBar();
    void sendConfigureEvent(Window win, int x, int y, int width, int height);
};

#endif // WINDOW_H
