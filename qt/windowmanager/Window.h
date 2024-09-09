#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QWidget>

class Window : public QMainWindow {
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

private:
    QWidget *centralWidget;
    QProcess *xtermProcess;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    QWidget *xtermWidget;

    bool resizing;
    bool dragging;
    QPoint dragStartPosition;
    QPoint resizeStartPosition;
    QSize resizeStartSize;

    void setupUI();
    void launchXTerm();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void toggleFullScreen();
};

#endif // WINDOW_H
