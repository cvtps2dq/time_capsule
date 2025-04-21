//
// Created by cv2 on 4/21/25.
//

#ifndef TIME_CAPSULE_MESSAGE_WIDGET_H
#define TIME_CAPSULE_MESSAGE_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include "chat_data.h"

class MessageWidget : public QWidget {
    Q_OBJECT
public:
    explicit MessageWidget(const Message& message, const Conversation& conv, QWidget* parent = nullptr);
    QSize sizeHint() const override;

private:
    void setupUi();
    void loadAuthorInfo();

    const Message& message_;
    const Conversation& conv_;
    QLabel* authorLabel_;
    QLabel* timeLabel_;
    QLabel* contentLabel_;
    QLabel* avatarLabel_;
};

#endif //TIME_CAPSULE_MESSAGE_WIDGET_H
