#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDialog>
#include <QWindow>
#include <QEvent>

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskBar(QWidget *parent = nullptr);
    void showPopup();
    void closePopup();
    void addMinimizedWindow(QWindow *window);
    void restoreMinimizedWindow(QWindow *window);
    bool isPopupVisible = false;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *popup;
    QPushButton *startButton;
    QPushButton *powerButton;
    void adjustSizeToScreen();
    void showPowerMenu();
    void closePowerMenu();
    void installEventFilter();
    bool powerMenuVisible = false;
    bool isDarkMode;
    bool isWindowVisible = true;
};

#endif // TASKBAR_H
