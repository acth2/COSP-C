#include "loginwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <cstdlib>

LoginWindow::LoginWindow(QWidget *parent)
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
}

void LoginWindow::onLoginClicked() {
    QString username = usernameField->text();
    QString password = passwordField->text();

    authenticateUser(username, password);
}

void LoginWindow::authenticateUser(const QString &username, const QString &password) {
    if (username == "root" && password == "password") {
        QMessageBox::information(this, "Login Successful", "Welcome!");

        system("startx /usr/bin/cwm");
        
        close();
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}
