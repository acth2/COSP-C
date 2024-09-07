#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QTextEdit>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QProcess>
#include <QPlainTextEdit>

class TerminalWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TerminalWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void toggleFullScreen();
    void windowedFullScreen();
    void handleTerminalOutput();      
    void handleTerminalErrorOutput();

private:
    QWidget *centralWidget;
    QWidget *topBar;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    QPlainTextEdit *terminalWidget;
    QProcess *terminalProcess;          
    bool isFullScreenMode;
    bool dragging;
    bool isFullMode;
    bool windowedFull;
    bool resizing;
    QPoint dragStartPosition;
    QSize resizeStartSize;
    QPoint resizeStartPosition;
    QString currentCommand;          

    void setupUI();
    void updateTopBarVisibility();
    void startTerminalProcess();
    void processInput();
    void sendCommandToTerminal(const QString &command);
    QString inputBuffer;
};

#endif // TERMINALWINDOW_H
