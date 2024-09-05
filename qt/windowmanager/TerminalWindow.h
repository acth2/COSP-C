#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QProcess>
#include <QMenuBar>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QPushButton>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);
    void toggleFullScreen();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QWidget *centralWidget;
    QProcess *terminalProcess;
    QVBoxLayout *layout;
    bool isFullScreenMode;
    QRect normalGeometry;

    void setupUI();
    void centerWindow();
};

#endif // TERMINALWINDOW_H
