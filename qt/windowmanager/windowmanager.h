#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QWidget>

class WindowManager : public QWidget {
    Q_OBJECT

public:
    explicit WindowManager(QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // WINDOWMANAGER_H
