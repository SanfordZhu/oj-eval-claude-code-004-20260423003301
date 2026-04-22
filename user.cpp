#include "user.h"
#include <algorithm>
#include <cctype>

User::User() : privilege(0) {}

User::User(const std::string& id, const std::string& pwd, const std::string& name, int priv)
    : userID(id), password(pwd), username(name), privilege(priv) {}

bool User::isValidUserID(const std::string& id) const {
    if (id.empty() || id.length() > 30) return false;
    for (char c : id) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

bool User::isValidPassword(const std::string& pwd) const {
    if (pwd.empty() || pwd.length() > 30) return false;
    for (char c : pwd) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

bool User::isValidUsername(const std::string& name) const {
    if (name.empty() || name.length() > 30) return false;
    for (char c : name) {
        if (c < 32 || c > 126) return false;
    }
    return true;
}

bool User::isValidPrivilege(int priv) const {
    return priv == 1 || priv == 3 || priv == 7;
}

void User::serialize(std::ofstream& out) const {
    size_t idLen = userID.length();
    size_t pwdLen = password.length();
    size_t nameLen = username.length();

    out.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
    out.write(userID.c_str(), idLen);
    out.write(reinterpret_cast<const char*>(&pwdLen), sizeof(pwdLen));
    out.write(password.c_str(), pwdLen);
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    out.write(username.c_str(), nameLen);
    out.write(reinterpret_cast<const char*>(&privilege), sizeof(privilege));
}

void User::deserialize(std::ifstream& in) {
    size_t idLen, pwdLen, nameLen;

    in.read(reinterpret_cast<char*>(&idLen), sizeof(idLen));
    userID.resize(idLen);
    in.read(&userID[0], idLen);

    in.read(reinterpret_cast<char*>(&pwdLen), sizeof(pwdLen));
    password.resize(pwdLen);
    in.read(&password[0], pwdLen);

    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    username.resize(nameLen);
    in.read(&username[0], nameLen);

    in.read(reinterpret_cast<char*>(&privilege), sizeof(privilege));
}

UserManager::UserManager(const std::string& filename) : dataFile(filename) {
    loadFromFile();
}

UserManager::~UserManager() {
    saveToFile();
}

int UserManager::findUser(const std::string& userID) const {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].getUserID() == userID) {
            return i;
        }
    }
    return -1;
}

void UserManager::saveToFile() {
    std::ofstream out(dataFile, std::ios::binary);
    if (!out) return;

    size_t count = users.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& user : users) {
        user.serialize(out);
    }
    out.close();
}

void UserManager::loadFromFile() {
    std::ifstream in(dataFile, std::ios::binary);
    if (!in) return;

    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    users.resize(count);
    for (size_t i = 0; i < count; ++i) {
        users[i].deserialize(in);
    }
    in.close();
}

bool UserManager::addUser(const std::string& userID, const std::string& password,
                         const std::string& username, int privilege) {
    if (findUser(userID) != -1) return false;

    User newUser(userID, password, username, privilege);
    if (!newUser.isValidUserID(userID) || !newUser.isValidPassword(password) ||
        !newUser.isValidUsername(username) || !newUser.isValidPrivilege(privilege)) {
        return false;
    }

    users.push_back(newUser);
    saveToFile();
    return true;
}

bool UserManager::deleteUser(const std::string& userID) {
    int index = findUser(userID);
    if (index == -1) return false;

    for (const auto& uid : loginStack) {
        if (uid == userID) return false;
    }

    users.erase(users.begin() + index);
    saveToFile();
    return true;
}

bool UserManager::login(const std::string& userID, const std::string& password) {
    int index = findUser(userID);
    if (index == -1) return false;

    const User& user = users[index];

    if (!password.empty() && user.getPassword() != password) {
        return false;
    }

    if (password.empty() && !currentUserID.empty()) {
        int currentPriv = getUserPrivilege(currentUserID);
        int targetPriv = user.getPrivilege();
        if (currentPriv <= targetPriv) {
            return false;
        }
    }

    loginStack.push_back(userID);
    currentUserID = userID;
    return true;
}

bool UserManager::logout() {
    if (loginStack.empty()) return false;

    loginStack.pop_back();
    if (loginStack.empty()) {
        currentUserID.clear();
    } else {
        currentUserID = loginStack.back();
    }
    return true;
}

bool UserManager::changePassword(const std::string& userID, const std::string& newPassword) {
    int index = findUser(userID);
    if (index == -1) return false;

    User& user = users[index];
    if (!user.isValidPassword(newPassword)) return false;

    user.setPassword(newPassword);
    saveToFile();
    return true;
}

int UserManager::getCurrentPrivilege() const {
    if (currentUserID.empty()) return 0;
    return getUserPrivilege(currentUserID);
}

bool UserManager::userExists(const std::string& userID) const {
    return findUser(userID) != -1;
}

int UserManager::getUserPrivilege(const std::string& userID) const {
    int index = findUser(userID);
    if (index == -1) return -1;
    return users[index].getPrivilege();
}

void UserManager::initializeRootUser() {
    if (!userExists("root")) {
        addUser("root", "sjtu", "root", 7);
    }
}