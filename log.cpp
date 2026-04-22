#include "log.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>

LogManager::LogManager(const std::string& finance, const std::string& log)
    : financeFile(finance), logFile(log) {
    loadFinanceFromFile();
    loadLogFromFile();
}

LogManager::~LogManager() {
    saveFinanceToFile();
    saveLogToFile();
}

void LogManager::saveFinanceToFile() {
    std::ofstream out(financeFile, std::ios::binary);
    if (!out) return;

    size_t count = financeRecords.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& record : financeRecords) {
        size_t opLen = record.operation.length();
        size_t userLen = record.userID.length();

        out.write(reinterpret_cast<const char*>(&opLen), sizeof(opLen));
        out.write(record.operation.c_str(), opLen);
        out.write(reinterpret_cast<const char*>(&record.amount), sizeof(record.amount));
        out.write(reinterpret_cast<const char*>(&userLen), sizeof(userLen));
        out.write(record.userID.c_str(), userLen);
        out.write(reinterpret_cast<const char*>(&record.timestamp), sizeof(record.timestamp));
    }
    out.close();
}

void LogManager::loadFinanceFromFile() {
    std::ifstream in(financeFile, std::ios::binary);
    if (!in) return;

    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    financeRecords.resize(count);
    for (size_t i = 0; i < count; ++i) {
        size_t opLen, userLen;

        in.read(reinterpret_cast<char*>(&opLen), sizeof(opLen));
        financeRecords[i].operation.resize(opLen);
        in.read(&financeRecords[i].operation[0], opLen);

        in.read(reinterpret_cast<char*>(&financeRecords[i].amount), sizeof(financeRecords[i].amount));

        in.read(reinterpret_cast<char*>(&userLen), sizeof(userLen));
        financeRecords[i].userID.resize(userLen);
        in.read(&financeRecords[i].userID[0], userLen);

        in.read(reinterpret_cast<char*>(&financeRecords[i].timestamp), sizeof(financeRecords[i].timestamp));
    }
    in.close();
}

void LogManager::saveLogToFile() {
    std::ofstream out(logFile, std::ios::binary);
    if (!out) return;

    size_t count = operationLogs.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& log : operationLogs) {
        size_t opLen = log.operation.length();
        size_t detLen = log.details.length();
        size_t userLen = log.userID.length();

        out.write(reinterpret_cast<const char*>(&opLen), sizeof(opLen));
        out.write(log.operation.c_str(), opLen);
        out.write(reinterpret_cast<const char*>(&detLen), sizeof(detLen));
        out.write(log.details.c_str(), detLen);
        out.write(reinterpret_cast<const char*>(&userLen), sizeof(userLen));
        out.write(log.userID.c_str(), userLen);
        out.write(reinterpret_cast<const char*>(&log.timestamp), sizeof(log.timestamp));
    }
    out.close();
}

void LogManager::loadLogFromFile() {
    std::ifstream in(logFile, std::ios::binary);
    if (!in) return;

    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    operationLogs.resize(count);
    for (size_t i = 0; i < count; ++i) {
        size_t opLen, detLen, userLen;

        in.read(reinterpret_cast<char*>(&opLen), sizeof(opLen));
        operationLogs[i].operation.resize(opLen);
        in.read(&operationLogs[i].operation[0], opLen);

        in.read(reinterpret_cast<char*>(&detLen), sizeof(detLen));
        operationLogs[i].details.resize(detLen);
        in.read(&operationLogs[i].details[0], detLen);

        in.read(reinterpret_cast<char*>(&userLen), sizeof(userLen));
        operationLogs[i].userID.resize(userLen);
        in.read(&operationLogs[i].userID[0], userLen);

        in.read(reinterpret_cast<char*>(&operationLogs[i].timestamp), sizeof(operationLogs[i].timestamp));
    }
    in.close();
}

void LogManager::addFinanceRecord(const std::string& operation, double amount, const std::string& userID) {
    financeRecords.emplace_back(operation, amount, userID);
    saveFinanceToFile();
}

void LogManager::addOperationLog(const std::string& operation, const std::string& details, const std::string& userID) {
    operationLogs.emplace_back(operation, details, userID);
    saveLogToFile();
}

std::pair<double, double> LogManager::getFinanceSummary(int count) const {
    double income = 0.0;
    double expenditure = 0.0;

    int start = 0;
    if (count > 0 && count < static_cast<int>(financeRecords.size())) {
        start = financeRecords.size() - count;
    }

    for (int i = start; i < static_cast<int>(financeRecords.size()); ++i) {
        if (financeRecords[i].operation == "buy") {
            income += financeRecords[i].amount;
        } else if (financeRecords[i].operation == "import") {
            expenditure += financeRecords[i].amount;
        }
    }

    return {income, expenditure};
}

std::vector<FinanceRecord> LogManager::getFinanceRecords(int count) const {
    if (count <= 0 || count >= static_cast<int>(financeRecords.size())) {
        return financeRecords;
    }

    std::vector<FinanceRecord> result;
    int start = financeRecords.size() - count;
    for (int i = start; i < static_cast<int>(financeRecords.size()); ++i) {
        result.push_back(financeRecords[i]);
    }
    return result;
}

std::vector<OperationLog> LogManager::getOperationLogs() const {
    return operationLogs;
}

std::string LogManager::generateFinanceReport() const {
    std::stringstream ss;
    auto [income, expenditure] = getFinanceSummary();

    ss << "=== FINANCE REPORT ===\n";
    ss << "Total Income: " << std::fixed << std::setprecision(2) << income << "\n";
    ss << "Total Expenditure: " << std::fixed << std::setprecision(2) << expenditure << "\n";
    ss << "Net Profit: " << std::fixed << std::setprecision(2) << (income - expenditure) << "\n";
    ss << "====================\n";

    return ss.str();
}

std::string LogManager::generateEmployeeReport(const std::string& employeeID) const {
    std::stringstream ss;
    std::map<std::string, int> operationCount;
    std::map<std::string, double> operationValue;

    for (const auto& record : financeRecords) {
        if (record.userID == employeeID) {
            operationCount[record.operation]++;
            operationValue[record.operation] += record.amount;
        }
    }

    ss << "=== EMPLOYEE REPORT FOR " << employeeID << " ===\n";
    for (const auto& op : operationCount) {
        ss << "Operation: " << op.first << "\n";
        ss << "  Count: " << op.second << "\n";
        ss << "  Total Value: " << std::fixed << std::setprecision(2) << operationValue[op.first] << "\n";
    }
    ss << "=====================================\n";

    return ss.str();
}

std::string LogManager::generateFullLog() const {
    std::stringstream ss;

    ss << "=== OPERATION LOGS ===\n";
    for (const auto& log : operationLogs) {
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&log.timestamp));
        ss << "[" << timeStr << "] " << log.userID << " - " << log.operation << ": " << log.details << "\n";
    }

    ss << "\n=== FINANCE LOGS ===\n";
    for (const auto& record : financeRecords) {
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&record.timestamp));
        ss << "[" << timeStr << "] " << record.userID << " - " << record.operation << ": "
           << std::fixed << std::setprecision(2) << record.amount << "\n";
    }

    return ss.str();
}