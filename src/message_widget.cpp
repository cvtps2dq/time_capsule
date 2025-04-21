#include "message_widget.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include <QPixmap>


MessageWidget::MessageWidget(const Message& message, const Conversation& conv, QWidget* parent)
        : QWidget(parent), message_(message), conv_(conv)
{
    setupUi();
    loadAuthorInfo();

    // Set size policy to expanding
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

QSize MessageWidget::sizeHint() const {
    // Calculate minimum width needed
    int width = parentWidget() ? parentWidget()->width() - 40 : 500;  // Account for margins

    // Calculate height needed for content
    contentLabel_->setFixedWidth(width - 60);  // Account for avatar and margins
    int height = contentLabel_->heightForWidth(contentLabel_->width()) +
                 authorLabel_->height() +
                 20;  // Add padding

    return QSize(width, qMax(height, 60));  // Ensure minimum height of avatar
}

void MessageWidget::setupUi() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);  // Add padding

    // Avatar
    avatarLabel_ = new QLabel(this);
    avatarLabel_->setFixedSize(40, 40);
    avatarLabel_->setStyleSheet("border-radius: 20px; background-color: #ddd;");
    mainLayout->addWidget(avatarLabel_);

    // Content area
    QVBoxLayout* contentLayout = new QVBoxLayout();
    contentLayout->setSpacing(5);

    // Header (author + time)
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(10);

    authorLabel_ = new QLabel(this);
    authorLabel_->setStyleSheet("font-weight: bold;");
    headerLayout->addWidget(authorLabel_);

    timeLabel_ = new QLabel(this);
    timeLabel_->setStyleSheet("color: #666; font-size: 10px;");
    headerLayout->addWidget(timeLabel_);
    headerLayout->addStretch();

    contentLayout->addLayout(headerLayout);

    // Message content
    contentLabel_ = new QLabel(this);
    contentLabel_->setWordWrap(true);
    contentLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contentLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    contentLabel_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    contentLayout->addWidget(contentLabel_);

    mainLayout->addLayout(contentLayout, 1);

    // Set minimum width to prevent too narrow messages
    setMinimumWidth(200);

    // Style based on author type
    if (message_.author_type == "user") {
        this->setStyleSheet("background-color: #e3f2fd; border-radius: 8px; padding: 8px;");
    } else {
        this->setStyleSheet("background-color: #f5f5f5; border-radius: 8px; padding: 8px;");
    }
}


void MessageWidget::loadAuthorInfo() {
    // Set author name
    QString authorName = QString::fromStdString(message_.author_name);
    if (authorName.isEmpty()) {
        authorName = message_.author_type == "user" ? "You" : "Xoul";
    }
    authorLabel_->setText(authorName);

    // Set timestamp if available
    if (!message_.timestamp.empty()) {
        timeLabel_->setText(QString::fromStdString(format_timestamp(message_.timestamp)));
    } else {
        timeLabel_->setText("");
    }

    // Set message content
    contentLabel_->setText(QString::fromStdString(message_.content));

    // Find avatar
    std::string avatar_url;
    if (message_.author_type == "user") {
        if (!conv_.personas.empty()) {
            avatar_url = conv_.personas[0].icon_url;
        }
    } else {
        if (!conv_.xouls.empty()) {
            avatar_url = conv_.xouls[0].icon_url;
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