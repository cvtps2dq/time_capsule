//
// Created by cv2 on 4/21/25.
//

#include "chat_data.h"
#include <fstream>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

std::string format_timestamp(const std::string& iso_timestamp) {
    std::tm tm = {};
    std::istringstream ss(iso_timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail()) {
        return iso_timestamp;
    }

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

Conversation load_conversation(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    json j;
    file >> j;

    Conversation conv;

    auto conv_json = j["conversation"];
    conv.conversation_id = conv_json["conversation_id"];
    conv.name = conv_json["name"];
    conv.icon_url = conv_json["icon_url"];
    conv.narrator_voice = conv_json["narrator_voice"].is_null() ? "" : conv_json["narrator_voice"];
    conv.auto_respond = conv_json["auto_respond"];
    conv.narrate = conv_json["narrate"];
    conv.greeting = conv_json["greeting"];
    conv.greeter = conv_json["greeter"].is_null() ? "" : conv_json["greeter"];
    conv.talking_style = conv_json["talking_style"];

    for (const auto& xoul_json : conv_json["xouls"]) {
        Xoul xoul;
        xoul.slug = xoul_json["slug"];
        xoul.name = xoul_json["name"];
        xoul.icon_url = xoul_json["icon_url"];
        xoul.voice_id = xoul_json["voice_id"].is_null() ? "" : xoul_json["voice_id"];
        xoul.talkativeness = xoul_json["talkativeness"];
        xoul.tagline = xoul_json["tagline"];
        xoul.age = xoul_json["age"];
        xoul.backstory = xoul_json["backstory"].is_null() ? "" : xoul_json["backstory"];
        xoul.gender = xoul_json["gender"];
        xoul.samples = xoul_json["samples"].is_null() ? "" : xoul_json["samples"];
        conv.xouls.push_back(xoul);
    }

    for (const auto& persona_json : conv_json["personas"]) {
        Persona persona;
        persona.slug = persona_json["slug"];
        persona.name = persona_json["name"];
        persona.icon_url = persona_json["icon_url"];
        persona.prompt = persona_json["prompt"];
        persona.user_slug = persona_json["user_slug"];
        persona.gender = persona_json["gender"];
        persona.privilege = persona_json["privilege"];
        conv.personas.push_back(persona);
    }

    conv.scenario.slug = conv_json["scenario"]["slug"];
    conv.scenario.name = conv_json["scenario"]["name"];
    for (const auto& prompt : conv_json["scenario"]["prompt"]) {
        conv.scenario.prompt.push_back(prompt);
    }
    for (const auto& icon : conv_json["scenario"]["icon_url"]) {
        conv.scenario.icon_url.push_back(icon);
    }

    for (const auto& msg_json : j["messages"]) {
        Message msg;
        msg.message_id = msg_json["message_id"];
        msg.conversation_id = msg_json["conversation_id"];
        msg.timestamp = msg_json["timestamp"];
        msg.status = msg_json["status"];
        msg.author_slug = msg_json["author_slug"];
        msg.author_type = msg_json["author_type"];
        msg.author_name = msg_json["author_name"];
        msg.content_type = msg_json["content_type"];
        msg.content = msg_json["content"];
        conv.messages.push_back(msg);
    }

    return conv;
}

ChatData load_chat_data(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        json j;
        try {
            file >> j;
        } catch (const json::exception& e) {
            throw std::runtime_error("Invalid JSON format: " + std::string(e.what()));
        }

        ChatData chat_data;
        chat_data.scenario = {"", "none", {}, {}};

        // Check if it's a group chat or single chat
        if (j.contains("messages") && !j["messages"].empty() && j["messages"][0].contains("author_type")) {
            // Group chat format
            chat_data.is_group_chat = true;
            try {
                chat_data.group_chat = load_conversation(filename);
            } catch (const std::exception& e) {
                throw std::runtime_error("Failed to load group chat: " + std::string(e.what()));
            }

            // Copy common fields with null checks
            chat_data.conversation_id = chat_data.group_chat.conversation_id;
            chat_data.name = chat_data.group_chat.name;
            chat_data.icon_url = chat_data.group_chat.icon_url;
            chat_data.xouls = chat_data.group_chat.xouls;
            chat_data.personas = chat_data.group_chat.personas;
            chat_data.scenario = chat_data.group_chat.scenario;
        } else {
            // Single chat format
            chat_data.is_group_chat = false;

            if (!j.contains("conversation")) {
                throw std::runtime_error("Missing 'conversation' object in JSON");
            }

            auto conv_json = j["conversation"];

            try {
                chat_data.conversation_id = conv_json.value("conversation_id", "");
                if (conv_json.contains("name") && !conv_json["name"].is_null()) {
                    chat_data.name = conv_json.value("name", "Untitled Conversation");
                } else {
                    chat_data.name = "Untitled Conversation";
                }

                if (conv_json.value("icon_url", "").empty()) { chat_data.icon_url = "";}
                chat_data.icon_url = conv_json.value("icon_url", "");
            } catch (const json::exception& e) {
                throw std::runtime_error("Error parsing conversation metadata: " + std::string(e.what()));
            }

            // Load xouls with error handling
            if (conv_json.contains("xouls")) {
                try {
                    for (const auto& xoul_json : conv_json["xouls"]) {
                        Xoul xoul;
                        xoul.slug = xoul_json.value("slug", "");
                        xoul.name = xoul_json.value("name", "Unknown Xoul");
                        xoul.icon_url = xoul_json.value("icon_url", "");
                        chat_data.xouls.push_back(xoul);
                    }
                } catch (const json::exception& e) {
                    throw std::runtime_error("Error parsing xouls: " + std::string(e.what()));
                }
            }

            // Load personas with error handling
            if (conv_json.contains("personas")) {
                try {
                    for (const auto& persona_json : conv_json["personas"]) {
                        Persona persona;
                        persona.slug = persona_json.value("slug", "");
                        persona.name = persona_json.value("name", "Unknown User");
                        persona.icon_url = persona_json.value("icon_url", "");
                        chat_data.personas.push_back(persona);
                    }
                } catch (const json::exception& e) {
                    throw std::runtime_error("Error parsing personas: " + std::string(e.what()));
                }
            }

            // Load scenario with error handling
            if (conv_json.contains("scenario")) {
                try {
                    bool flag = false;
                    auto scenario_json = conv_json["scenario"];
                    if (scenario_json.contains("name") && !scenario_json["name"].is_null()) {
                        chat_data.scenario.name = scenario_json.value("name", "none");
                    }
                    if (scenario_json.contains("slug") && !scenario_json["slug"].is_null()) {
                        chat_data.scenario.name = scenario_json.value("slug", "none");
                    }
                    if (scenario_json.contains("prompt") && !scenario_json["prompt"].is_null()) {
                        for (const auto& prompt : scenario_json["prompt"]) {
                            if (prompt.is_string()) {
                                chat_data.scenario.prompt.push_back(prompt);
                            }
                        }
                    }
                } catch (const json::exception& e) {
                    throw std::runtime_error("Error parsing scenario: " + std::string(e.what()));
                }
            }

            // Load messages with error handling
            if (j.contains("messages")) {
                try {
                    for (const auto& msg_json : j["messages"]) {
                        SimpleMessage msg;
                        msg.role = msg_json.value("role", "");
                        msg.content = msg_json.value("content", "");
                        msg.timestamp = msg_json.value("timestamp", "");
                        chat_data.simple_messages.push_back(msg);
                    }
                } catch (const json::exception& e) {
                    throw std::runtime_error("Error parsing messages: " + std::string(e.what()));
                }
            }
        }

        return chat_data;
    } catch (const std::exception& e) {
        // Wrap the error with file information
        throw std::runtime_error("Error loading " + filename + ": " + e.what());
    }
}