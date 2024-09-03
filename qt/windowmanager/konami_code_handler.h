#ifndef KONAMI_CODE_HANDLER_H
#define KONAMI_CODE_HANDLER_H

#include <QObject>
#include <QKeyEvent>

class KonamiCodeHandler : public QObject {
    Q_OBJECT

public:
    explicit KonamiCodeHandler(QObject *parent = nullptr);

signals:
    void konamiCodeEntered();

public slots:
    void handleKeyPress(QKeyEvent *event);

private:
    QString currentSequence;
    const QString konamiCode = "38384040373937386665";
};

#endif // KONAMI_CODE_HANDLER_H
