#ifndef USERINTERACTRIGHT_H
#define USERINTERACTRIGHT_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QPoint>
#include <QWindow>

class UserInteractRight : public QWidget {
    Q_OBJECT

public:
    explicit UserInteractRight(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void button1Clicked();
    void button2Clicked();

private:
    void setupUI();
    void applyStyles();
    void closeIfClickedOutside(QMouseEvent *event);

    void onWindowClick(QWindow *window);
    void onMouseMove(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);

    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QLabel *textLabel;
    bool isDarkMode;
    bool resizeMode;
    QPoint initialClickPos;
    bool waitingForClick = false;
    QWindow *currentResizingWindow = nullptr;
    QWidget *currentResizingWidget;
    WindowManager *windowManagerInstance;

};

#endif // USERINTERACTRIGHT_H
