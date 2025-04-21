#include "message_widget.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QPixmap>

MessageWidget::MessageWidget(const Message& message, const Conversation& conv, QWidget* parent)
        : QWidget(parent), message_(message), conv_(conv) {
    setupUi();
    loadAuthorInfo();
}

void MessageWidget::setupUi() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    // Avatar placeholder
    avatarLabel_ = new QLabel(this);
    avatarLabel_->setFixedSize(40, 40);
    avatarLabel_->setStyleSheet("border-radius: 20px; background-color: #ddd;");
    mainLayout->addWidget(avatarLabel_);

    // Message content
    QVBoxLayout* contentLayout = new QVBoxLayout();

    QHBoxLayout* headerLayout = new QHBoxLayout();
    authorLabel_ = new QLabel(this);
    authorLabel_->setStyleSheet("font-weight: bold;");
    headerLayout->addWidget(authorLabel_);

    timeLabel_ = new QLabel(this);
    timeLabel_->setStyleSheet("color: #666; font-size: 10px;");
    headerLayout->addWidget(timeLabel_);
    headerLayout->addStretch();

    contentLayout->addLayout(headerLayout);

    contentLabel_ = new QLabel(this);
    contentLabel_->setWordWrap(true);
    contentLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contentLayout->addWidget(contentLabel_);

    mainLayout->addLayout(contentLayout, 1);

    // Style based on author type
    if (message_.author_type == "user") {
        this->setStyleSheet("background-color: #e3f2fd; border-radius: 8px; padding: 8px;");
    } else {
        this->setStyleSheet("background-color: #f5f5f5; border-radius: 8px; padding: 8px;");
    }
}

void MessageWidget::loadAuthorInfo() {
    // Convert std::string to QString
    authorLabel_->setText(QString::fromStdString(message_.author_name));
    timeLabel_->setText(QString::fromStdString(format_timestamp(message_.timestamp)));
    contentLabel_->setText(QString::fromStdString(message_.content));

    // Try to find avatar URL
    std::string avatar_url;

    if (message_.author_type == "user") {
        for (const auto& persona : conv_.personas) {
            if (persona.slug == message_.author_slug) {
                avatar_url = persona.icon_url;
                break;
            }
        }
    } else {
        for (const auto& xoul : conv_.xouls) {
            if (xoul.slug == message_.author_slug) {
                avatar_url = xoul.icon_url;
                break;
            }
        }
    }

    if (!avatar_url.empty()) {
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError) {
                QPixmap pixmap;
                pixmap.loadFromData(reply->readAll());
                if (!pixmap.isNull()) {
                    avatarLabel_->setPixmap(pixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    avatarLabel_->setStyleSheet("border-radius: 20px;");
                }
            }
            reply->deleteLater();
            sender()->deleteLater();
        });

        // Convert std::string to QUrl
        manager->get(QNetworkRequest(QUrl(QString::fromStdString(avatar_url))));
    }
}