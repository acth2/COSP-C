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

private:
    void setupUI();
    void startXterm();
    QWidget *topBar;

    QWidget *centralWidget;
    QWidget *xtermWidget;
    QProcess *xtermProcess;
    bool isFullScreenMode;

    void updateTopBarVisibility();
};

#endif // TERMINALWINDOW_H
