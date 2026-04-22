#include "system.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <regex>

BookstoreSystem::BookstoreSystem() : running(true) {
    userManager.initializeRootUser();
}

BookstoreSystem::~BookstoreSystem() {
    bookManager.clearSelection();
}

std::vector<std::string> BookstoreSystem::parseCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;

    for (size_t i = 0; i < command.length(); ++i) {
        char c = command[i];

        if (c == '"') {
            if (inQuotes) {
                // End of quoted string
                token += '"';
                tokens.push_back(token);
                token.clear();
                inQuotes = false;
            } else {
                // Start of quoted string
                inQuotes = true;
                token += '"';
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

std::string BookstoreSystem::extractQuotedString(const std::string& str) {
    if (str.length() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.length() - 2);
    }
    return str;
}

void BookstoreSystem::printInvalid() {
    std::cout << "Invalid" << std::endl;
}

void BookstoreSystem::processCommand(const std::string& command) {
    if (command.empty() || std::all_of(command.begin(), command.end(), ::isspace)) {
        return;
    }

    std::vector<std::string> args = parseCommand(command);
    if (args.empty()) return;

    const std::string& cmd = args[0];

    // Debug output
    // std::cerr << "DEBUG: Command: " << cmd << ", Args: " << args.size() << std::endl;

    if (cmd == "quit" || cmd == "exit") {
        running = false;
    } else if (cmd == "su") {
        handleSu(args);
    } else if (cmd == "logout") {
        handleLogout(args);
    } else if (cmd == "register") {
        handleRegister(args);
    } else if (cmd == "passwd") {
        handlePasswd(args);
    } else if (cmd == "useradd") {
        handleUseradd(args);
    } else if (cmd == "delete") {
        handleDelete(args);
    } else if (cmd == "show") {
        if (args.size() > 1 && args[1] == "finance") {
            handleShowFinance(args);
        } else {
            handleShow(args);
        }
    } else if (cmd == "buy") {
        handleBuy(args);
    } else if (cmd == "select") {
        handleSelect(args);
    } else if (cmd == "modify") {
        handleModify(args);
    } else if (cmd == "import") {
        handleImport(args);
    } else if (cmd == "log") {
        handleLog(args);
    } else if (cmd == "report") {
        if (args.size() > 1 && args[1] == "finance") {
            handleReportFinance(args);
        } else if (args.size() > 1 && args[1] == "employee") {
            handleReportEmployee(args);
        } else {
            printInvalid();
        }
    } else {
        printInvalid();
    }
}

void BookstoreSystem::handleSu(const std::vector<std::string>& args) {
    if (args.size() < 2 || args.size() > 3) {
        printInvalid();
        return;
    }

    std::string password;
    if (args.size() == 3) {
        password = args[2];
    }

    if (!userManager.login(args[1], password)) {
        printInvalid();
    }
}

void BookstoreSystem::handleLogout(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        printInvalid();
        return;
    }

    if (!userManager.logout()) {
        printInvalid();
    }
}

void BookstoreSystem::handleRegister(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        printInvalid();
        return;
    }

    // Debug output
    // std::cerr << "DEBUG: Registering user " << args[1] << " with username " << args[3] << std::endl;

    if (!userManager.addUser(args[1], args[2], args[3], 1)) {
        printInvalid();
    }
}

void BookstoreSystem::handlePasswd(const std::vector<std::string>& args) {
    if (args.size() < 3 || args.size() > 4) {
        printInvalid();
        return;
    }

    std::string currentPassword;
    std::string newPassword;

    if (args.size() == 3) {
        newPassword = args[2];
    } else {
        currentPassword = args[2];
        newPassword = args[3];
    }

    if (!userManager.userExists(args[1])) {
        printInvalid();
        return;
    }

    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 7 && currentPassword.empty()) {
        printInvalid();
        return;
    }

    if (!userManager.changePassword(args[1], newPassword)) {
        printInvalid();
    }
}

void BookstoreSystem::handleUseradd(const std::vector<std::string>& args) {
    if (args.size() != 5) {
        printInvalid();
        return;
    }

    int privilege = std::stoi(args[3]);
    if (privilege != 1 && privilege != 3 && privilege != 7) {
        printInvalid();
        return;
    }

    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 3 || currentPriv <= privilege) {
        printInvalid();
        return;
    }

    if (!userManager.addUser(args[1], args[2], args[4], privilege)) {
        printInvalid();
    }
}

void BookstoreSystem::handleDelete(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        printInvalid();
        return;
    }

    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 7) {
        printInvalid();
        return;
    }

    if (!userManager.deleteUser(args[1])) {
        printInvalid();
    }
}

void BookstoreSystem::handleShow(const std::vector<std::string>& args) {
    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 1) {
        printInvalid();
        return;
    }

    std::string isbn, name, author, keyword;

    if (args.size() > 1) {
        for (size_t i = 1; i < args.size(); ++i) {
            std::string arg = args[i];
            if (arg.find("-ISBN=") == 0) {
                isbn = arg.substr(6);
            } else if (arg.find("-name=") == 0) {
                name = extractQuotedString(arg.substr(6));
            } else if (arg.find("-author=") == 0) {
                author = extractQuotedString(arg.substr(8));
            } else if (arg.find("-keyword=") == 0) {
                keyword = extractQuotedString(arg.substr(9));
            } else {
                printInvalid();
                return;
            }
        }
    }

    std::vector<Book> books = bookManager.showBooks(isbn, name, author, keyword);
    if (books.empty()) {
        std::cout << std::endl;
        return;
    }

    for (const auto& book : books) {
        std::cout << book.getISBN() << "\t" << book.getBookName() << "\t"
                  << book.getAuthor() << "\t" << book.getKeyword() << "\t"
                  << std::fixed << std::setprecision(2) << book.getPrice() << "\t"
                  << book.getQuantity() << std::endl;
    }
}

void BookstoreSystem::handleBuy(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        printInvalid();
        return;
    }

    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 1) {
        printInvalid();
        return;
    }

    int quantity;
    try {
        quantity = std::stoi(args[2]);
    } catch (...) {
        printInvalid();
        return;
    }

    double totalPrice;
    if (!bookManager.buyBook(args[1], quantity, totalPrice)) {
        printInvalid();
        return;
    }

    logManager.addFinanceRecord("buy", totalPrice, userManager.getCurrentUserID());
    std::cout << std::fixed << std::setprecision(2) << totalPrice << std::endl;
}

void BookstoreSystem::handleSelect(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        printInvalid();
        return;
    }

    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 3) {
        printInvalid();
        return;
    }

    if (!bookManager.selectBook(args[1])) {
        printInvalid();
    }
}

void BookstoreSystem::handleModify(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        printInvalid();
        return;
    }

    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 3) {
        printInvalid();
        return;
    }

    if (!bookManager.isBookSelected()) {
        printInvalid();
        return;
    }

    std::string isbn, name, author, keyword;
    double price = -1;

    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg.find("-ISBN=") == 0) {
            isbn = arg.substr(6);
        } else if (arg.find("-name=") == 0) {
            name = extractQuotedString(arg.substr(6));
        } else if (arg.find("-author=") == 0) {
            author = extractQuotedString(arg.substr(8));
        } else if (arg.find("-keyword=") == 0) {
            keyword = extractQuotedString(arg.substr(9));
        } else if (arg.find("-price=") == 0) {
            try {
                price = std::stod(arg.substr(7));
            } catch (...) {
                printInvalid();
                return;
            }
        } else {
            printInvalid();
            return;
        }
    }

    if (!bookManager.modifyBook(isbn, name, author, keyword, price)) {
        printInvalid();
    }
}

void BookstoreSystem::handleImport(const std::vector<std::string>& args) {
    if (args.size() != 3) {
        printInvalid();
        return;
    }

    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 3) {
        printInvalid();
        return;
    }

    int quantity;
    double totalCost;
    try {
        quantity = std::stoi(args[1]);
        totalCost = std::stod(args[2]);
    } catch (...) {
        printInvalid();
        return;
    }

    if (!bookManager.importBook(quantity, totalCost)) {
        printInvalid();
        return;
    }

    logManager.addFinanceRecord("import", totalCost, userManager.getCurrentUserID());
}

void BookstoreSystem::handleShowFinance(const std::vector<std::string>& args) {
    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 7) {
        printInvalid();
        return;
    }

    int count = -1;
    if (args.size() == 3) {
        try {
            count = std::stoi(args[2]);
        } catch (...) {
            printInvalid();
            return;
        }
    } else if (args.size() != 2) {
        printInvalid();
        return;
    }

    auto [income, expenditure] = logManager.getFinanceSummary(count);
    std::cout << "+ " << std::fixed << std::setprecision(2) << income
              << " - " << std::fixed << std::setprecision(2) << expenditure << std::endl;
}

void BookstoreSystem::handleLog(const std::vector<std::string>& args) {
    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 7) {
        printInvalid();
        return;
    }

    if (args.size() != 1) {
        printInvalid();
        return;
    }

    std::cout << logManager.generateFullLog();
}

void BookstoreSystem::handleReportFinance(const std::vector<std::string>& args) {
    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 7) {
        printInvalid();
        return;
    }

    if (args.size() != 2) {
        printInvalid();
        return;
    }

    std::cout << logManager.generateFinanceReport();
}

void BookstoreSystem::handleReportEmployee(const std::vector<std::string>& args) {
    int currentPriv = userManager.getCurrentPrivilege();
    if (currentPriv < 7) {
        printInvalid();
        return;
    }

    if (args.size() != 2) {
        printInvalid();
        return;
    }

    std::cout << logManager.generateEmployeeReport("all");
}

void BookstoreSystem::run() {
    std::string line;
    while (running && std::getline(std::cin, line)) {
        processCommand(line);
    }
}