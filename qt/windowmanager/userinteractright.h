#ifndef USERINTERACTRIGHT_H
#define USERINTERACTRIGHT_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>

class UserInteractRight : public QWidget {
    Q_OBJECT

public:
    explicit UserInteractRight(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QPushButton *exitButton;
    QLabel *textLabel;

    void closeIfClickedOutside(const QPoint &pos);
};

#endif // USERINTERACTRIGHT_H
