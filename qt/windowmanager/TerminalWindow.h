#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QWidget>
#include <QPushButton>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QFocusEvent>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    void setupUI();
    void launchXTerm();
    void updateTopBarVisibility();
    void toggleFullScreen();

    QWidget *centralWidget;
    QWidget *topBar;
    QWidget *xtermWidget;
    QProcess *xtermProcess;
    bool isFullScreenMode;
    bool dragging;
    QPoint dragStartPosition;
};

#endif // TERMINALWINDOW_H
