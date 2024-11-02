#include <iostream>
#include <cstdlib>

void sayIt(const std::string& str) {
    // Use the festival command line interface to speak
    std::string cmd = "echo -n \"" + str + "\" | festival --tts";
    int result = system(cmd.c_str());

    if (result != 0) {
        std::cout << "Failed to speak." << std::endl;
    }
}

int main() {
    sayIt("Hello, how are you?");
    return 0;
}
