#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QWidget>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void toggleFullScreen();
    void focusInEvent(QFocusEvent* event);

private:
    void setupUI();
    void startXterm();
    void launchXTerm();
    QWidget *topBar;

    QProcess *terminalProcess;
    QWidget *terminalWidget;

    QWidget *centralWidget;
    QWidget *xtermWidget;
    QProcess *xtermProcess;
    bool isFullScreenMode;

    int 

    void updateTopBarVisibility();
};

#endif // TERMINALWINDOW_H
