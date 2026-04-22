#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>
#include <fstream>
#include <ctime>

struct FinanceRecord {
    std::string operation;
    double amount;
    std::string userID;
    std::time_t timestamp;

    FinanceRecord() : amount(0.0), timestamp(0) {}
    FinanceRecord(const std::string& op, double amt, const std::string& user)
        : operation(op), amount(amt), userID(user), timestamp(std::time(nullptr)) {}
};

struct OperationLog {
    std::string operation;
    std::string details;
    std::string userID;
    std::time_t timestamp;

    OperationLog() : timestamp(0) {}
    OperationLog(const std::string& op, const std::string& det, const std::string& user)
        : operation(op), details(det), userID(user), timestamp(std::time(nullptr)) {}
};

class LogManager {
private:
    std::vector<FinanceRecord> financeRecords;
    std::vector<OperationLog> operationLogs;
    std::string financeFile;
    std::string logFile;

    void saveFinanceToFile();
    void loadFinanceFromFile();
    void saveLogToFile();
    void loadLogFromFile();

public:
    LogManager(const std::string& finance = "finance.dat", const std::string& log = "logs.dat");
    ~LogManager();

    void addFinanceRecord(const std::string& operation, double amount, const std::string& userID);
    void addOperationLog(const std::string& operation, const std::string& details, const std::string& userID);

    std::pair<double, double> getFinanceSummary(int count = -1) const;
    std::vector<FinanceRecord> getFinanceRecords(int count = -1) const;
    std::vector<OperationLog> getOperationLogs() const;

    std::string generateFinanceReport() const;
    std::string generateEmployeeReport(const std::string& employeeID) const;
    std::string generateFullLog() const;
};

#endif