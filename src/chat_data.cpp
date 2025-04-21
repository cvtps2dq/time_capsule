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