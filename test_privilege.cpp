#include "user.h"
#include <iostream>

int main() {
    UserManager um;
    um.initializeRootUser();

    std::cout << "Before login - Current privilege: " << um.getCurrentPrivilege() << std::endl;

    um.login("root", "sjtu");
    std::cout << "After login - Current privilege: " << um.getCurrentPrivilege() << std::endl;

    return 0;
}