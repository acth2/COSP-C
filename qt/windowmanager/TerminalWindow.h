#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);
    void toggleFullScreen();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QWidget *terminalWidget;
    QProcess *terminalProcess;
    QVBoxLayout *layout;
    QWidget *customTitleBar;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    bool isFullScreenMode;
    QRect normalGeometry;

    void setupUI();
    void centerWindow();
    void setupCustomTitleBar();
};

#endif // TERMINALWINDOW_H
