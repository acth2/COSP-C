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
#include <QEvent>

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskBar(QWidget *parent = nullptr);

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
    void showPopup();
    void closePopup();
    void showPowerMenu();
    void closePowerMenu();
    void installEventFilter();
    bool isPopupVisible = false;
    bool powerMenuVisible = false;
    bool showPopupK;
    bool isDarkMode;
};

#endif // TASKBAR_H
