//
// Created by cv2 on 4/21/25.
//

#ifndef TIME_CAPSULE_CHAT_DATA_H
#define TIME_CAPSULE_CHAT_DATA_H

#ifndef CHAT_DATA_H
#define CHAT_DATA_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct Xoul {
    std::string slug;
    std::string name;
    std::string icon_url;
    std::string voice_id;
    double talkativeness;
    std::string tagline;
    int age;
    std::string backstory;
    std::string gender;
    std::string samples;
};

struct Persona {
    std::string slug;
    std::string name;
    std::string icon_url;
    std::string prompt;
    std::string user_slug;
    std::string gender;
    std::string privilege;
};

struct Scenario {
    std::string slug;
    std::string name;
    std::vector<std::string> prompt;
    std::vector<std::string> icon_url;
};

struct Message {
    uint64_t message_id;
    std::string conversation_id;
    std::string timestamp;
    std::string status;
    std::string author_slug;
    std::string author_type;
    std::string author_name;
    std::string content_type;
    std::string content;
};

struct Conversation {
    std::string conversation_id;
    std::string name;
    std::string icon_url;
    std::string narrator_voice;
    bool auto_respond;
    bool narrate;
    std::vector<Xoul> xouls;
    std::vector<Persona> personas;
    Scenario scenario;
    std::string greeting;
    std::string talking_style;
    std::vector<Message> messages;
    std::string greeter;
};

struct SimpleMessage {
    std::string role;  // "user" or "assistant"
    std::string content;
    std::string timestamp;
};

struct ChatData {
    bool is_group_chat;
    Conversation group_chat;
    std::vector<SimpleMessage> simple_messages;
    std::vector<Xoul> xouls;
    std::vector<Persona> personas;
    Scenario scenario;

    // Common fields
    std::string conversation_id;
    std::string name;
    std::string icon_url;
};

ChatData load_chat_data(const std::string& filename);

Conversation load_conversation(const std::string& filename);
std::string format_timestamp(const std::string& iso_timestamp);

#endif // CHAT_DATA_H

#endif //TIME_CAPSULE_CHAT_DATA_H
