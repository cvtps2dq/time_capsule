//
// Created by cv2 on 4/21/25.
//

#include "main_window.h"
#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    // Open file from command line if provided
    if (argc > 1) {
        QString filename = argv[1];
        QFileInfo fileInfo(filename);
        if (fileInfo.exists()) {
            // Try to detect chat type by filename or content
            // For simplicity, we'll just try both
            try {
                mainWindow.loadGroupChat(filename);
            } catch (...) {
                try {
                    mainWindow.loadSingleChat(filename);
                } catch (...) {
                    QMessageBox::warning(&mainWindow, "Warning",
                                         "Could not determine chat type. Please use the menu to open the file.");
                }
            }
        }
    }

    return app.exec();
}