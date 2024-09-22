#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();

private:
    void authenticateUser(const QString &username, const QString &password);
    QLineEdit *usernameField;
    QLineEdit *passwordField;
    QPushButton *loginButton;

    QString executeCommand(const QString &command);
};

#endif // LOGINWINDOW_H
