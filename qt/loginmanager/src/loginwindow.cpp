#include "loginwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>
#include <QProcess>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *userLabel = new QLabel("Username:");
    usernameField = new QLineEdit;
    QLabel *passLabel = new QLabel("Password:");
    passwordField = new QLineEdit;
    passwordField->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Login");

    outputArea = new QTextEdit;
    outputArea->setReadOnly(true);

    layout->addWidget(userLabel);
    layout->addWidget(usernameField);
    layout->addWidget(passLabel);
    layout->addWidget(passwordField);
    layout->addWidget(loginButton);
    layout->addWidget(outputArea);

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
    QString command = QString("a2wmlagent %1 %2").arg(username, password);

    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, this, [this, process]() {
        QString output = process->readAllStandardOutput();
        outputArea->append(output);
    });
    connect(process, &QProcess::readyReadStandardError, this, [this, process]() {
        QString errorOutput = process->readAllStandardError();
        outputArea->append(errorOutput);
    });

    process->start(command);
}
