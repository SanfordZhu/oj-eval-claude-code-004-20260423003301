#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

std::vector<std::string> parseCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::stringstream ss(command);
    std::string token;

    while (ss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

int main() {
    std::string line;
    std::cout << "Enter command: ";
    std::getline(std::cin, line);

    std::cout << "You entered: '" << line << "'" << std::endl;

    auto tokens = parseCommand(line);
    std::cout << "Parsed tokens: " << tokens.size() << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "  [" << i << "] '" << tokens[i] << "'" << std::endl;
    }

    return 0;
}