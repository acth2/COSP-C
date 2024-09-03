#ifndef USERINTERACTRIGHT_H
#define USERINTERACTRIGHT_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>

class UserInteractRight : public QWidget {
    Q_OBJECT

public:
    explicit UserInteractRight(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVBoxLayout *layout;
    QLabel *textLabel;
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
};

#endif // USERINTERACTRIGHT_H
