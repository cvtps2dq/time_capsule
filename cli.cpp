#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Data structures to hold the chat information
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
    std::string greeter;
    bool auto_respond;
    bool narrate;
    std::vector<Xoul> xouls;
    std::vector<Persona> personas;
    Scenario scenario;
    std::string greeting;
    std::string talking_style;
    std::vector<Message> messages;
};

// Function to parse timestamp into a more readable format
std::string format_timestamp(const std::string& iso_timestamp) {
    std::tm tm = {};
    std::istringstream ss(iso_timestamp);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail()) {
        return iso_timestamp; // Return original if parsing fails
    }

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Function to load and parse the JSON file
Conversation load_conversation(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    json j;
    file >> j;

    Conversation conv;

    // Parse conversation metadata
    auto conv_json = j["conversation"];
    conv.conversation_id = conv_json["conversation_id"];
    conv.name = conv_json["name"];
    conv.icon_url = conv_json["icon_url"];
    conv.narrator_voice = conv_json["narrator_voice"].is_null() ? "" : conv_json["narrator_voice"];
    conv.auto_respond = conv_json["auto_respond"];
    conv.narrate = conv_json["narrate"];
    conv.greeting = conv_json["greeting"];
    conv.talking_style = conv_json["talking_style"];

    // Parse xouls
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

    // Parse personas
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

    // Parse scenario
    conv.scenario.slug = conv_json["scenario"]["slug"];
    conv.scenario.name = conv_json["scenario"]["name"];
    for (const auto& prompt : conv_json["scenario"]["prompt"]) {
        conv.scenario.prompt.push_back(prompt);
    }
    for (const auto& icon : conv_json["scenario"]["icon_url"]) {
        conv.scenario.icon_url.push_back(icon);
    }

    // Parse messages
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

// Function to display the conversation in the console
void display_conversation(const Conversation& conv) {
    std::cout << "Conversation: " << conv.name << "\n";
    std::cout << "ID: " << conv.conversation_id << "\n";
    std::cout << "Participants:\n";

    // Display personas (users)
    for (const auto& persona : conv.personas) {
        std::cout << "  [User] " << persona.name << " (" << persona.gender << ")\n";
    }

    // Display xouls (AI characters)
    for (const auto& xoul : conv.xouls) {
        std::cout << "  [Xoul] " << xoul.name << " (" << xoul.gender << ", " << xoul.age << ")\n";
    }

    std::cout << "\nScenario: " << conv.scenario.name << "\n";
    if (!conv.scenario.prompt.empty()) {
        std::cout << "Prompt: " << conv.scenario.prompt[0] << "\n";
    }

    std::cout << "\n--- Chat Log ---\n";

    // Display greeting if exists
    if (!conv.greeting.empty()) {
        std::string greeter_name;
        for (const auto& xoul : conv.xouls) {
            if (xoul.slug == conv.greeter) {
                greeter_name = xoul.name;
                break;
            }
        }
        std::cout << format_timestamp("") << " " << greeter_name << ": " << conv.greeting << "\n";
    }

    // Display all messages
    for (const auto& msg : conv.messages) {
        std::cout << format_timestamp(msg.timestamp) << " " << msg.author_name << ": " << msg.content << "\n";
    }
}

//int main(int argc, char* argv[]) {
//    if (argc != 2) {
//        std::cerr << "Usage: " << argv[0] << " <xoul_chat_json_file>\n";
//        return 1;
//    }
//
//    try {
//        Conversation conv = load_conversation(argv[1]);
//        display_conversation(conv);
//    } catch (const std::exception& e) {
//        std::cerr << "Error: " << e.what() << "\n";
//        return 1;
//    }
//
//    return 0;
//}