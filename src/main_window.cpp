#include "main_window.h"
#include "message_widget.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QAction>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    setupMenus();
    setWindowTitle("Xoul Chat Viewer");
    resize(800, 600);
}

void MainWindow::setupUi() {
    centralWidget_ = new QWidget(this);
    mainLayout_ = new QVBoxLayout(centralWidget_);

    scrollArea_ = new QScrollArea(centralWidget_);
    scrollArea_->setWidgetResizable(true);

    scrollContent_ = new QWidget(scrollArea_);
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setAlignment(Qt::AlignTop);

    scrollArea_->setWidget(scrollContent_);
    mainLayout_->addWidget(scrollArea_);

    setCentralWidget(centralWidget_);
}

void MainWindow::setupMenus() {
    fileMenu_ = menuBar()->addMenu("File");

    openGroupChatAction_ = new QAction("Open Group Chat", this);
    connect(openGroupChatAction_, &QAction::triggered, this, &MainWindow::openGroupChat);
    fileMenu_->addAction(openGroupChatAction_);

    openSingleChatAction_ = new QAction("Open Single Chat", this);
    connect(openSingleChatAction_, &QAction::triggered, this, &MainWindow::openSingleChat);
    fileMenu_->addAction(openSingleChatAction_);
}

void MainWindow::openGroupChat() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Group Chat JSON", "", "JSON Files (*.json)");
    if (!filename.isEmpty()) {
        loadGroupChat(filename);
    }
}

void MainWindow::openSingleChat() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Single Chat JSON", "", "JSON Files (*.json)");
    if (!filename.isEmpty()) {
        loadSingleChat(filename);
    }
}

void MainWindow::loadGroupChat(const QString& filename) {
    try {
        clearConversation();
        Conversation conv = load_conversation(filename.toStdString());
        displayGroupChat(conv);
        statusBar()->showMessage("Loaded group chat: " + filename, 3000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to load group chat: ") + e.what());
    }
}

void MainWindow::loadSingleChat(const QString& filename) {
    try {
        clearConversation();
        ChatData chat_data = load_chat_data(filename.toStdString());
        if (chat_data.is_group_chat) {
            QMessageBox::warning(this, "Warning", "This appears to be a group chat. Please use 'Open Group Chat' instead.");
            return;
        }
        displaySingleChat(chat_data);
        statusBar()->showMessage("Loaded single chat: " + filename, 3000);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to load single chat: ") + e.what());
    }
}

void MainWindow::displayGroupChat(const Conversation& conv) {
    clearConversation();

    // Create a container widget for better layout control
    QWidget* contentContainer = new QWidget(scrollContent_);
    QVBoxLayout* containerLayout = new QVBoxLayout(contentContainer);
    containerLayout->setContentsMargins(15, 15, 15, 15);
    containerLayout->setSpacing(15);

    // Header section
    QLabel* titleLabel = new QLabel(QString("<h2 style='margin-bottom: 10px;'>%1</h2>").arg(
            conv.name.empty() ? "Group Chat" : conv.name.c_str()), contentContainer);
    titleLabel->setWordWrap(true);
    containerLayout->addWidget(titleLabel);

    // Scenario section
    if (!conv.scenario.name.empty() && conv.scenario.name != "none") {
        QLabel* scenarioLabel = new QLabel(
                QString("<div style='margin-bottom: 10px;'><b>Scenario:</b> %1</div>").arg(conv.scenario.name.c_str()),
                contentContainer);
        scenarioLabel->setWordWrap(true);
        scenarioLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        containerLayout->addWidget(scenarioLabel);
    }

    // Participants section
    QStringList participants;
    for (const auto& persona : conv.personas) {
        participants << QString("%1 (User)").arg(persona.name.c_str());
    }
    for (const auto& xoul : conv.xouls) {
        participants << QString("%1 (Xoul)").arg(xoul.name.c_str());
    }

    if (!participants.isEmpty()) {
        QLabel* participantsLabel = new QLabel(
                QString("<div style='margin-bottom: 15px;'><b>Participants:</b> %1</div>").arg(participants.join(", ")),
                contentContainer);
        participantsLabel->setWordWrap(true);
        containerLayout->addWidget(participantsLabel);
    }

    // Messages container
    QWidget* messagesContainer = new QWidget(contentContainer);
    QVBoxLayout* messagesLayout = new QVBoxLayout(messagesContainer);
    messagesLayout->setContentsMargins(0, 0, 0, 0);
    messagesLayout->setSpacing(10);
    messagesLayout->setAlignment(Qt::AlignTop);

    // Greeting message
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

        MessageWidget* greetingWidget = new MessageWidget(greeting_msg, conv, messagesContainer);
        messagesLayout->addWidget(greetingWidget);
    }

    // Regular messages
    for (const auto& msg : conv.messages) {
        MessageWidget* msgWidget = new MessageWidget(msg, conv, messagesContainer);
        messagesLayout->addWidget(msgWidget);
    }

    containerLayout->addWidget(messagesContainer);
    scrollLayout_->addWidget(contentContainer);
    scrollLayout_->addStretch();

    // Ensure proper sizing
    contentContainer->adjustSize();
    scrollArea_->setWidgetResizable(true);
}

void MainWindow::displaySingleChat(const ChatData& chat_data) {
    clearConversation();

    // Create a container widget for better layout control
    QWidget* contentContainer = new QWidget(scrollContent_);
    QVBoxLayout* containerLayout = new QVBoxLayout(contentContainer);
    containerLayout->setContentsMargins(15, 15, 15, 15);
    containerLayout->setSpacing(15);

    // Header section
    QLabel* titleLabel = new QLabel(QString("<h2 style='margin-bottom: 10px;'>%1</h2>").arg(
            chat_data.name.empty() ? "Private Chat" : chat_data.name.c_str()), contentContainer);
    titleLabel->setWordWrap(true);
    containerLayout->addWidget(titleLabel);

    // Scenario section
    if (!chat_data.scenario.name.empty() && chat_data.scenario.name != "none") {
        QLabel* scenarioLabel = new QLabel(
                QString("<div style='margin-bottom: 10px;'><b>Scenario:</b> %1</div>").arg(chat_data.scenario.name.c_str()),
                contentContainer);
        scenarioLabel->setWordWrap(true);
        scenarioLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        containerLayout->addWidget(scenarioLabel);
    }

    // Participants section
    QStringList participants;
    for (const auto& persona : chat_data.personas) {
        participants << QString("%1 (User)").arg(persona.name.c_str());
    }
    for (const auto& xoul : chat_data.xouls) {
        participants << QString("%1 (Xoul)").arg(xoul.name.c_str());
    }

    if (!participants.isEmpty()) {
        QLabel* participantsLabel = new QLabel(
                QString("<div style='margin-bottom: 15px;'><b>Participants:</b> %1</div>").arg(participants.join(", ")),
                contentContainer);
        participantsLabel->setWordWrap(true);
        containerLayout->addWidget(participantsLabel);
    }

    // Messages container
    QWidget* messagesContainer = new QWidget(contentContainer);
    QVBoxLayout* messagesLayout = new QVBoxLayout(messagesContainer);
    messagesLayout->setContentsMargins(0, 0, 0, 0);
    messagesLayout->setSpacing(10);
    messagesLayout->setAlignment(Qt::AlignTop);

    // All messages
    for (const auto& msg : chat_data.simple_messages) {
        Message display_msg;
        display_msg.content = msg.content;
        display_msg.timestamp = msg.timestamp;

        if (msg.role == "user") {
            display_msg.author_type = "user";
            display_msg.author_name = chat_data.personas.empty() ? "You" : chat_data.personas[0].name;
        } else {
            display_msg.author_type = "llm";
            display_msg.author_name = chat_data.xouls.empty() ? "Xoul" : chat_data.xouls[0].name;
        }

        // Create a dummy conversation for the message widget
        Conversation dummy_conv;
        dummy_conv.xouls = chat_data.xouls;
        dummy_conv.personas = chat_data.personas;

        MessageWidget* msgWidget = new MessageWidget(display_msg, dummy_conv, messagesContainer);
        messagesLayout->addWidget(msgWidget);
    }

    containerLayout->addWidget(messagesContainer);
    scrollLayout_->addWidget(contentContainer);
    scrollLayout_->addStretch();

    // Ensure proper sizing
    contentContainer->adjustSize();
    scrollArea_->setWidgetResizable(true);
}

void MainWindow::clearConversation() {
    QLayoutItem* item;
    while ((item = scrollLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}