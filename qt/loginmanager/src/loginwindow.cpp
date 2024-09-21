#include "loginwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <cstdlib>
#include <crypt.h>
#include <shadow.h>
#include <unistd.h>

LoginWindow::LoginWindow(QWidget *parent) {
    : QWidget(parent) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *userLabel = new QLabel("Username:");
    usernameField = new QLineEdit;
    QLabel *passLabel = new QLabel("Password:");
    passwordField = new QLineEdit;
    passwordField->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Login");

    layout->addWidget(userLabel);
    layout->addWidget(usernameField);
    layout->addWidget(passLabel);
    layout->addWidget(passwordField);
    layout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);

    QSize windowSize = sizeHint();
    int screenWidth = QApplication::desktop()->screen()->width();
    int screenHeight = QApplication::desktop()->screen()->height();
    move((screenWidth - windowSize.width()) / 2, (screenHeight - windowSize.height()) / 2);
    resize(windowSize);
}

void LoginWindow::onLoginClicked() {
    QString username = usernameField->text();
    QString password = passwordField->text();

    authenticateUser(username, password);
}

void LoginWindow::authenticateUser(const QString &username, const QString &password) {
    struct spwd *userInfo = getspnam(username.toStdString().c_str());
    if (!userInfo) {
        QTimer::singleShot(3000, this, [=]() {
            QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
        });
        return;
    }

    const char *hashedPassword = crypt(password.toStdString().c_str(), userInfo->sp_pwdp);

    if (hashedPassword && (QString(hashedPassword) == QString(userInfo->sp_pwdp))) {
        QMessageBox::information(this, "Login Successful", "Welcome!");
        system("startx /usr/bin/cwm");
        close();
    } else {
        QTimer::singleShot(3000, this, [=]() {
            QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
        });
    }
}
