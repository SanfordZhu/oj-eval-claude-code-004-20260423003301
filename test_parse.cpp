#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> parseCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;

    for (size_t i = 0; i < command.length(); ++i) {
        char c = command[i];

        if (c == '"') {
            if (inQuotes) {
                // End of quoted string
                tokens.push_back(token);
                token.clear();
                inQuotes = false;
            } else {
                // Start of quoted string
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                inQuotes = true;
            }
        } else if (c == ' ' && !inQuotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

int main() {
    std::string cmd = "modify -name=\"Test Book\" -author=\"Test Author\" -keyword=\"test|book\" -price=15.99";
    auto tokens = parseCommand(cmd);

    std::cout << "Command: " << cmd << std::endl;
    std::cout << "Tokens: " << tokens.size() << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "  [" << i << "] '" << tokens[i] << "'" << std::endl;
    }

    return 0;
}