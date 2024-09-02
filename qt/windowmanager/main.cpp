#include <QApplication>
#include <QMessageBox>
#include <cstdlib>
#include "windowmanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    const char* display = std::getenv("DISPLAY");
    if (display && *display != '\0') {
        WindowManager manager;
        manager.setGeometry(0, 0, 800, 600);  
        manager.setWindowTitle("My Window Manager");
        manager.show();

        return app.exec();
    } else {
        std::cerr << "Erreur : Aucun serveur X ne sont détectés... Veuillez lancer dans un environnement graphique." << std::endl;
        return 1;
    }
}
