#ifndef TERMINALWINDOW_H
#define TERMINALWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QPlainTextEdit> 
#include <QMouseEvent>
#include <QKeyEvent>
#include <QProcess> 

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

private:
    QWidget *centralWidget;
    QWidget *topBar;
    QPushButton *closeButton;
    QPushButton *fullscreenButton;
    QPlainTextEdit *terminalWidget;
    bool isFullScreenMode;
    bool dragging;
    bool isFullMode;
    bool windowedFull;
    bool resizing;
    QPoint dragStartPosition;
    QSize resizeStartSize;
    QPoint resizeStartPosition;

    QProcess *terminalProcess; 
    QString currentCommand;   

    void setupUI();
    void updateTopBarVisibility();
    
    void startTerminalProcess();
    void handleTerminalOutput();
    void processInput();
};

#endif // TERMINALWINDOW_H
