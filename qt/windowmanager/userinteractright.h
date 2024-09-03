#ifndef USERINTERACTRIGHT_H
#define USERINTERACTRIGHT_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLabel>

class WindowManager;

class UserInteractRight : public QWidget {
    Q_OBJECT

public:
    explicit UserInteractRight(WindowManager *windowManager, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    WindowManager *windowManager;
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QLabel *textLabel;
    bool isMouseInside;

    void setupUI();
    void checkOutsideClick(QMouseEvent *event);
    void logOpening(); 
};

#endif // USERINTERACTRIGHT_H
