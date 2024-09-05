#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QWidget>
#include <QProcess>

class TerminalWindow : public QWidget {
    Q_OBJECT

public:
    TerminalWindow(QWidget *parent = nullptr);
    ~TerminalWindow();

private:
    QProcess *terminalProcess;
};

#endif // TERMINALWINDOW_H
