//
// Created by cv2 on 4/21/25.
//

#include "main_window.h"
#include "message_widget.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    setWindowTitle("Xoul Chat Viewer");
    resize(800, 600);
}

void MainWindow::setupUi() {
    // Create central widget
    centralWidget_ = new QWidget(this);
    mainLayout_ = new QVBoxLayout(centralWidget_);

    // Create menu
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* fileMenu = menuBar->addMenu("File");
    QAction* openAction = fileMenu->addAction("Open");
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    setMenuBar(menuBar);

    // Create scroll area for messages
    scrollArea_ = new QScrollArea(centralWidget_);
    scrollArea_->setWidgetResizable(true);

    scrollContent_ = new QWidget(scrollArea_);
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setAlignment(Qt::AlignTop);

    scrollArea_->setWidget(scrollContent_);
    mainLayout_->addWidget(scrollArea_);

    setCentralWidget(centralWidget_);
}

void MainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Xoul Chat JSON", "", "JSON Files (*.json)");
    if (!filename.isEmpty()) {
        loadConversation(filename);
    }
}

void MainWindow::loadConversation(const QString& filename) {
    try {
        clearConversation();
        currentConversation_ = load_conversation(filename.toStdString());
        displayConversation(currentConversation_);
        statusBar()->showMessage("Loaded: " + filename, 3000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to load file: ") + e.what());
    }
}

void MainWindow::clearConversation() {
    QLayoutItem* item;
    while ((item = scrollLayout_->takeAt(0)) != nullptr) {  // Fixed while condition
        delete item->widget();
        delete item;
    }
}

void MainWindow::displayConversation(const Conversation& conv) {
    // Add conversation header
    QLabel* titleLabel = new QLabel(QString("<h1>%1</h1>").arg(conv.name.c_str()), scrollContent_);
    scrollLayout_->addWidget(titleLabel);

    // Add scenario info
    if (!conv.scenario.name.empty()) {
        QLabel* scenarioLabel = new QLabel(QString("<b>Scenario:</b> %1").arg(conv.scenario.name.c_str()), scrollContent_);
        scrollLayout_->addWidget(scenarioLabel);
    }

    // Add participants
    QStringList participants;
    for (const auto& persona : conv.personas) {
        participants << QString("%1 (User)").arg(persona.name.c_str());
    }
    for (const auto& xoul : conv.xouls) {
        participants << QString("%1 (Xoul)").arg(xoul.name.c_str());
    }

    QLabel* participantsLabel = new QLabel(QString("<b>Participants:</b> %1").arg(participants.join(", ")), scrollContent_);
    scrollLayout_->addWidget(participantsLabel);

    scrollLayout_->addSpacing(20);

    // Add greeting if exists
    if (!conv.greeting.empty()) {
        std::string greeter_name;
        for (const auto& xoul : conv.xouls) {
            if (xoul.slug == conv.greeter) {
                greeter_name = xoul.name;
                break;
            }
        }

        Message greeting_msg;
        greeting_msg.author_name = greeter_name;
        greeting_msg.author_type = "llm";
        greeting_msg.content = conv.greeting;
        greeting_msg.timestamp = "";

        MessageWidget* greetingWidget = new MessageWidget(greeting_msg, conv, scrollContent_);
        scrollLayout_->addWidget(greetingWidget);
    }

    // Add all messages
    for (const auto& msg : conv.messages) {
        MessageWidget* msgWidget = new MessageWidget(msg, conv, scrollContent_);
        scrollLayout_->addWidget(msgWidget);
    }

    // Add spacer to push content up
    scrollLayout_->addStretch();
}