#include "user.h"
#include <iostream>

int main() {
    UserManager um;

    std::cout << "Initializing root user..." << std::endl;
    um.initializeRootUser();

    std::cout << "Checking if root exists: " << um.userExists("root") << std::endl;
    std::cout << "Root privilege: " << um.getUserPrivilege("root") << std::endl;

    std::cout << "\nTesting login..." << std::endl;
    bool result = um.login("root", "sjtu");
    std::cout << "Login result: " << result << std::endl;
    std::cout << "Current privilege: " << um.getCurrentPrivilege() << std::endl;

    return 0;
}