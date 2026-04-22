#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <fstream>

class User {
private:
    std::string userID;
    std::string password;
    std::string username;
    int privilege;

public:
    User();
    User(const std::string& id, const std::string& pwd, const std::string& name, int priv);

    const std::string& getUserID() const { return userID; }
    const std::string& getPassword() const { return password; }
    const std::string& getUsername() const { return username; }
    int getPrivilege() const { return privilege; }

    void setPassword(const std::string& pwd) { password = pwd; }
    void setUsername(const std::string& name) { username = name; }
    void setPrivilege(int priv) { privilege = priv; }

    bool isValidUserID(const std::string& id) const;
    bool isValidPassword(const std::string& pwd) const;
    bool isValidUsername(const std::string& name) const;
    bool isValidPrivilege(int priv) const;

    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);
};

class UserManager {
private:
    std::vector<User> users;
    std::vector<std::string> loginStack;
    std::string currentUserID;
    std::string dataFile;

    int findUser(const std::string& userID) const;
    void saveToFile();
    void loadFromFile();

public:
    UserManager(const std::string& filename = "users.dat");
    ~UserManager();

    bool addUser(const std::string& userID, const std::string& password,
                 const std::string& username, int privilege);
    bool deleteUser(const std::string& userID);
    bool login(const std::string& userID, const std::string& password);
    bool logout();
    bool changePassword(const std::string& userID, const std::string& newPassword);
    bool verifyPassword(const std::string& userID, const std::string& password) const;

    int getCurrentPrivilege() const;
    const std::string& getCurrentUserID() const { return currentUserID; }
    bool userExists(const std::string& userID) const;
    int getUserPrivilege(const std::string& userID) const;

    void initializeRootUser();
};

#endif