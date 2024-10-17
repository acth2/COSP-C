#include "loginwindow.h"
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QCryptographicHash>
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <cstring>
#include <iostream>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    usernameLineEdit = new QLineEdit(this);
    usernameLineEdit->setPlaceholderText("Username");
    layout->addWidget(usernameLineEdit);

    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordLineEdit);

    QPushButton *loginButton = new QPushButton("Login", this);
    layout->addWidget(loginButton);

    statusLabel = new QLabel(this);
    layout->addWidget(statusLabel);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);

    centerWindow();
    setCursorInPasswordField();
}

LoginWindow::~LoginWindow() {
}

void LoginWindow::handleLogin() {
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();

    if (authenticate(username, password)) {
        statusLabel->setText("Login successful!");
        startSession(username);
    } else {
        statusLabel->setText("Authentication failed.");
    }
}

bool LoginWindow::authenticate(const QString &username, const QString &password) {
    struct passwd *pw = getpwnam(username.toStdString().c_str());
    if (!pw) {
        return false;
    }

    struct spwd *sp = getspnam(username.toStdString().c_str());
    if (!sp) {
        return false;
    }

    char *encrypted = crypt(password.toStdString().c_str(), sp->sp_pwdp);
    if (!encrypted) {
        return false;
    }

    return strcmp(encrypted, sp->sp_pwdp) == 0;
}

void LoginWindow::startSession(const QString &username) {
    struct passwd *pw = getpwnam(username.toStdString().c_str());
    if (!pw) {
        QMessageBox::critical(this, "Error", "Failed to start session.");
        return;
    }

    setenv("USER", pw->pw_name, 1);
    setenv("HOME", pw->pw_dir, 1);
    setenv("SHELL", pw->pw_shell, 1);
    chdir(pw->pw_dir);

    execl(pw->pw_shell, pw->pw_shell, nullptr);

    QMessageBox::critical(this, "Error", "Failed to start shell session.");
}

void LoginWindow::centerWindow() {
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    int windowWidth = this->width();
    int windowHeight = this->height();

    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    this->move(x, y);
}

void LoginWindow::setCursorInPasswordField() {
    passwordLineEdit->setFocus();
    passwordLineEdit->setCursorPosition(passwordLineEdit->text().length());
}
