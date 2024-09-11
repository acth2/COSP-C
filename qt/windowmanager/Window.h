#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QWidget>
#include <QPushButton>

class Window : public QMainWindow {
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();
    void setXorgAppWindow(WId windowId);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QWidget *taskbarWidget;
    QWidget *xtermWidget;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    QWidget *centralWidget;
    QProcess *xtermProcess;

    bool resizing = false;
    bool dragging = false;
    QPoint resizeStartPosition;
    QSize resizeStartSize;
    QPoint dragStartPosition;

    void setupUI();
    void launchXTerm();
    void attachTaskbar(WId windowId);
    void toggleFullScreen();
};

#endif // WINDOW_H
