#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QProcess>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QTimer>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);

private slots:
    void launchXTerm();
    void resizeXTerm();
    void toggleFullScreen();

private:
    void setupUI();
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QWidget *centralWidget;
    QWidget *xtermWidget;
    QProcess *xtermProcess;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    bool isFullScreenMode;
    QPoint dragStartPosition;
    bool dragging;
    bool resizing;
    QPoint resizeStartPosition;
    QSize resizeStartSize;
};

#endif // TERMINALWINDOW_H
