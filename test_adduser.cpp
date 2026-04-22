#include "user.h"
#include <iostream>

int main() {
    UserManager um;
    um.initializeRootUser();

    std::cout << "Testing addUser..." << std::endl;
    bool result = um.addUser("user1", "pass1", "UserOne", 1);
    std::cout << "Result: " << result << std::endl;

    std::cout << "User exists: " << um.userExists("user1") << std::endl;

    return 0;
}