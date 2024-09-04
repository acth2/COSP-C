#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QWidget>
#include <QProcess>
#include <QVBoxLayout>
#include <QWidget>

class TerminalWindow : public QWidget {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);
    ~TerminalWindow();

private:
    QProcess *terminalProcess;
    QWidget *containerWidget;
};

#endif // TERMINALWINDOW_H
