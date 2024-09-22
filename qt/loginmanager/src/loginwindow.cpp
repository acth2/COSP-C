#include "loginwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <array>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
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

QString LoginWindow::executeCommand(const QString &command) {
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.toStdString().c_str(), "r"), pclose);
    if (!pipe) return "popen() failed!";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return QString::fromStdString(result);
}

void LoginWindow::authenticateUser(const QString &username, const QString &password) {
    QString command = "echo " + username + ":" + password + " | /usr/bin/login";

    QString result = executeCommand(command);

    if (result.contains("success")) {
        QMessageBox::information(this, "Login Successful", "Welcome!");
    } else {
        QTimer::singleShot(3000, this, [=]() {
            QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
        });
    }
}
