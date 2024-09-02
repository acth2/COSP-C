#ifndef KONAMI_CODE_HANDLER_H
#define KONAMI_CODE_HANDLER_H

#include <QObject>
#include <QKeyEvent>
#include <QProcess>

class KonamiCodeHandler : public QObject {
    Q_OBJECT

public:
    explicit KonamiCodeHandler(QObject *parent = nullptr);

    void keyPressEvent(QKeyEvent *event);

private:
    void openConsole();
    void closeConsole();

    static const int konamiCode[];
    static const int konamiCodeLength;
    int currentCodeIndex;
    bool consoleOpened;
    QProcess *consoleProcess;
};

#endif // KONAMI_CODE_HANDLER_H
