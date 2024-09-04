#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QScreen>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskBar(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void showPopup();
    void closePopup();

private:
    QLabel *popup;
    QPushButton *startButton;
    void adjustSizeToScreen();
    void checkClickOutsidePopup(const QPoint &pos);
    bool isPopupVisible;
};

#endif // TASKBAR_H
