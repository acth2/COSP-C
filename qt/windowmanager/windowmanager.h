#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QWidget>
#include <QLabel>

class WindowManager : public QWidget {
    Q_OBJECT

public:
    explicit WindowManager(QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QString backgroundImagePath;
    QLabel *logLabel;
    void appendLog(const QString &message);
};

#endif // WINDOWMANAGER_H
