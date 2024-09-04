#ifndef TASKBAR_H
#define TASKBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>

class TaskBar : public QWidget {
    Q_OBJECT

public:
    explicit TaskBar(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    QLabel *popup;
    QPushButton *startButton;
    void adjustSizeToScreen();
    void hidePopup();
    bool popupVisible = false;
    void showPopup();
    void closePopup();
    void installEventFilter();
    bool isPopupVisible = false;
};

#endif // TASKBAR_H

