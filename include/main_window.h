//
// Created by cv2 on 4/21/25.
//
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFileDialog>
#include "chat_data.h"
#include <QStatusBar>

// Forward declarations
class QAction;
class QMenu;
class QLabel;
class QVBoxLayout;
class QScrollArea;

class QAction;
class QMenu;

class MainWindow : public QMainWindow {
Q_OBJECT


public:
    explicit MainWindow(QWidget* parent = nullptr);
    void loadGroupChat(const QString& filename);
    void loadSingleChat(const QString& filename);

private slots:
    void openGroupChat();
    void openSingleChat();

private:
    void setupUi();
    void setupMenus();
    void displayGroupChat(const Conversation& conv);
    void displaySingleChat(const ChatData& chat_data);
    void clearConversation();

    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
    QScrollArea* scrollArea_;
    QWidget* scrollContent_;
    QVBoxLayout* scrollLayout_;

    QMenu* fileMenu_;
    QAction* openGroupChatAction_;
    QAction* openSingleChatAction_;
};

#endif // MAIN_WINDOW_H
