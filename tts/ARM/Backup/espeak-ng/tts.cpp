#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

void list_voices() {
    std::cout << "Available voices:" << std::endl;
    std::system("espeak-ng -q -vlist");
}

std::string select_voice(const std::string& default_voice, const std::vector<std::string>& voice_options) {
    if (voice_options.empty()) {
        return default_voice;
    }

    std::cout << "Enter the number of the voice you want to use (or '0' for '" << default_voice << "'):" << std::endl;

    for (size_t i=0; i < voice_options.size(); ++i) {
        std::cout << "[" << (i+1) << "] " << voice_options[i] << std::endl;
    }

    int choice;
    std::cin >> choice;

    if (choice <= 0 || static_cast<size_t>(choice-1) >= voice_options.size()) {
        return default_voice;
    } else {
        return voice_options[choice-1];
    }
}

void sayIt(const std::string& text, const std::string& voice) {
    std::string cmd="espeak-ng -v '" + voice + "' \"" + text + "\"";
    int result=system(cmd.c_str());

    if (result != 0) {
        std::cout << "Failed to speak." << std::endl;
    }
}

int main() {
    list_voices();

    std::vector<std::string> voices;
    std::string default_voice="en";  // Default voice name, can be changed as per your preference
    std::cout << "Enter the number of the voice you want to use (or '0' for '" << default_voice << "'):" << std::endl;

    for (;;) {
        int choice;
        std::cin >> choice;

        if (choice == 0) {
            break;
        } else if (choice <= 0 || static_cast<size_t>(choice-1) >= voices.size()) {
            continue;
        } else {
            default_voice=select_voice(default_voice, voices);
            break;
        }
    }

    std::string text;
    std::cout << "Enter the text you want to say:" << std::endl;
    std::getline(std::cin, text);

    sayIt(text, default_voice);

    return 0;
}
