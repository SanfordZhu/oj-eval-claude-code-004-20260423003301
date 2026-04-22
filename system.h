#ifndef SYSTEM_H
#define SYSTEM_H

#include "user.h"
#include "book.h"
#include "log.h"
#include <string>
#include <vector>
#include <sstream>

class BookstoreSystem {
private:
    UserManager userManager;
    BookManager bookManager;
    LogManager logManager;
    bool running;

    std::vector<std::string> parseCommand(const std::string& command);
    bool validateString(const std::string& str, const std::string& type);

    void handleSu(const std::vector<std::string>& args);
    void handleLogout(const std::vector<std::string>& args);
    void handleRegister(const std::vector<std::string>& args);
    void handlePasswd(const std::vector<std::string>& args);
    void handleUseradd(const std::vector<std::string>& args);
    void handleDelete(const std::vector<std::string>& args);

    void handleShow(const std::vector<std::string>& args);
    void handleBuy(const std::vector<std::string>& args);
    void handleSelect(const std::vector<std::string>& args);
    void handleModify(const std::vector<std::string>& args);
    void handleImport(const std::vector<std::string>& args);

    void handleShowFinance(const std::vector<std::string>& args);
    void handleLog(const std::vector<std::string>& args);
    void handleReportFinance(const std::vector<std::string>& args);
    void handleReportEmployee(const std::vector<std::string>& args);

    void printInvalid();
    std::string extractQuotedString(const std::string& str);

public:
    BookstoreSystem();
    ~BookstoreSystem();

    void run();
    void processCommand(const std::string& command);
};

#endif