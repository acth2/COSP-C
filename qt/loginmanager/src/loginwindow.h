#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QString>
#include <QPushButton>

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    hashPassword(const QString &password);

private slots:
    void onLoginClicked();

private:
    QLineEdit *usernameField;
    QLineEdit *passwordField;
    QPushButton *loginButton;

    void authenticateUser(const QString &username, const QString &password);
};

#endif // LOGINWINDOW_H
