//
// Created by cv2 on 4/21/25.
//

#include "../include/main_window.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    // Open file from command line if provided
    if (argc > 1 && argv[1] != nullptr) {
        mainWindow.loadConversation(argv[1]);
    }

    return app.exec();
}