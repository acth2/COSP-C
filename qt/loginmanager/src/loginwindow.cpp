#include "loginwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QCryptographicHash>
#include <QPushButton>
#include <QCursor>
#include <QProcess>
#include <cstdlib>
#include <crypt.h>
#include <shadow.h>
#include <unistd.h>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    messageLabel = new QLabel();
    messageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(messageLabel);

    QLabel *userLabel = new QLabel("Username:");
    userLabel->setAlignment(Qt::AlignCenter);
    usernameField = new QLineEdit;
    
    QLabel *passLabel = new QLabel("Password:");
    passLabel->setAlignment(Qt::AlignCenter);
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

    QTimer::singleShot(100, this, [=]() {
        QCursor::setPos(QCursor::pos().x() + (screenWidth - windowSize.width()) / 2 + windowSize.width() / 2,
                         QCursor::pos().y() + (screenHeight - windowSize.height()) / 2 + windowSize.height() / 2);
    });
}

void LoginWindow::onLoginClicked() {
    QString username = usernameField->text();
    QString password = passwordField->text();

    authenticateUser(username, password);
}

void LoginWindow::showMessage(const QString &message, bool isError) {
    messageLabel->setText(message);
    messageLabel->setStyleSheet(isError ? "color: red;" : "color: green;");
}

void LoginWindow::authenticateUser(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) {
        QTimer::singleShot(3000, this, [=]() {
            showMessage("Username and password cannot be empty.", true);
        });
        return;
    }

    struct spwd *userInfo = getspnam(username.toStdString().c_str());
    if (!userInfo) {
        QTimer::singleShot(3000, this, [=]() {
            showMessage("Invalid username or password.", true);
        });
        return;
    }

    const char *hashedPassword = crypt(password.toStdString().c_str(), userInfo->sp_pwdp);

    if (hashedPassword && QString(hashedPassword) == QString(userInfo->sp_pwdp)) {
        showMessage("Welcome!", false);
    } else {
        QTimer::singleShot(3000, this, [=]() {
            showMessage("Invalid username or password.", true);
        });
    }
}
