#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void handleLogin();

private:
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QLabel *statusLabel;

    bool authenticate(const QString &username, const QString &password);
    void startSession(const QString &username);
};

#endif // LOGINWINDOW_H
