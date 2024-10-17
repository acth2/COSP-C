#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

/**
 * @class LoginWindow
 * @brief The LoginWindow class handles the login interface and authentication logic.
 */
class LoginWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for LoginWindow.
     * @param parent The parent widget. Defaults to nullptr.
     */
    explicit LoginWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor for LoginWindow.
     */
    ~LoginWindow();

private slots:
    /**
     * @brief Slot that handles the login button click event.
     * Triggers the authentication process when the user clicks the "Login" button.
     */
    void handleLogin();

private:
    // UI components
    QLineEdit *usernameLineEdit;  ///< Line edit for entering the username
    QLineEdit *passwordLineEdit;  ///< Line edit for entering the password
    QLabel *statusLabel;          ///< Label to show authentication status messages

    /**
     * @brief Authenticates the user by checking the provided username and password.
     * @param username The username entered by the user.
     * @param password The password entered by the user.
     * @return true if the authentication is successful, false otherwise.
     */
    bool authenticate(const QString &username, const QString &password);

    /**
     * @brief Starts a user session by launching the user's default shell upon successful login.
     * @param username The username of the authenticated user.
     */
    void startSession(const QString &username);

    /**
     * @brief Centers the login window on the screen.
     * Adjusts the position of the login window to the center of the primary screen.
     */
    void centerWindow();

    /**
     * @brief Sets the cursor focus on the password input field.
     * Ensures the cursor is positioned in the password field when the window is shown.
     */
    void setCursorInPasswordField();
};

#endif // LOGINWINDOW_H
