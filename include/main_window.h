//
// Created by cv2 on 4/21/25.
//

#ifndef TIME_CAPSULE_MAIN_WINDOW_H
#define TIME_CAPSULE_MAIN_WINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFileDialog>
#include "chat_data.h"

#include <QMainWindow>
#include "chat_data.h"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    void loadConversation(const QString& filename);

private slots:
    void openFile();

private:
    void setupUi();
    void displayConversation(const Conversation& conv);
    void clearConversation();

    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;
    Conversation currentConversation_;
};

#endif //TIME_CAPSULE_MAIN_WINDOW_H
