#ifndef USERINTERACTRIGHT_H
#define USERINTERACTRIGHT_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>

class UserInteractRight : public QWidget {
    Q_OBJECT

public:
    explicit UserInteractRight(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void button1Clicked();
private:
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QLabel *textLabel;
    bool isMousePressed = false;

    void setupUI();
};

#endif // USERINTERACTRIGHT_H
