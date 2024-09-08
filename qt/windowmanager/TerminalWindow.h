#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QWidget>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QKeyEvent>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setupUI();
    void launchXTerm();
    void updateTopBarVisibility();
    void toggleFullScreen();

    QWidget *topBar;
    QWidget *centralWidget;
    QWidget *xtermWidget;

    QProcess *xtermProcess;
    bool isFullScreenMode;
    bool dragging;
    QPoint dragStartPosition;
};

#endif // TERMINALWINDOW_H
