#include "book.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <iomanip>

Book::Book() : price(0.0), quantity(0) {}

Book::Book(const std::string& isbn, const std::string& name, const std::string& auth,
           const std::string& key, double pr, int qty)
    : ISBN(isbn), bookName(name), author(auth), keyword(key), price(pr), quantity(qty) {}

bool Book::isValidISBN(const std::string& isbn) const {
    if (isbn.empty() || isbn.length() > 20) return false;
    for (char c : isbn) {
        if (c < 32 || c > 126) return false;
    }
    return true;
}

bool Book::isValidBookName(const std::string& name) const {
    if (name.empty() || name.length() > 60) return false;
    for (char c : name) {
        if (c < 32 || c > 126 || c == '"') return false;
    }
    return true;
}

bool Book::isValidAuthor(const std::string& auth) const {
    if (auth.empty() || auth.length() > 60) return false;
    for (char c : auth) {
        if (c < 32 || c > 126 || c == '"') return false;
    }
    return true;
}

bool Book::isValidKeyword(const std::string& key) const {
    if (key.empty() || key.length() > 60) return false;
    for (char c : key) {
        if (c < 32 || c > 126 || c == '"') return false;
    }

    std::vector<std::string> keywords = getKeywords();
    std::set<std::string> uniqueKeywords;
    for (const auto& k : keywords) {
        if (k.empty()) return false;
        if (uniqueKeywords.count(k)) return false;
        uniqueKeywords.insert(k);
    }
    return true;
}

bool Book::isValidPrice(double pr) const {
    return pr >= 0;
}

bool Book::isValidQuantity(int qty) const {
    return qty >= 0 && qty <= 2147483647;
}

std::vector<std::string> Book::getKeywords() const {
    std::vector<std::string> result;
    std::stringstream ss(keyword);
    std::string token;

    while (std::getline(ss, token, '|')) {
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    return result;
}

bool Book::hasKeyword(const std::string& key) const {
    std::vector<std::string> keywords = getKeywords();
    return std::find(keywords.begin(), keywords.end(), key) != keywords.end();
}

void Book::serialize(std::ofstream& out) const {
    size_t isbnLen = ISBN.length();
    size_t nameLen = bookName.length();
    size_t authLen = author.length();
    size_t keyLen = keyword.length();

    out.write(reinterpret_cast<const char*>(&isbnLen), sizeof(isbnLen));
    out.write(ISBN.c_str(), isbnLen);
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    out.write(bookName.c_str(), nameLen);
    out.write(reinterpret_cast<const char*>(&authLen), sizeof(authLen));
    out.write(author.c_str(), authLen);
    out.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
    out.write(keyword.c_str(), keyLen);
    out.write(reinterpret_cast<const char*>(&price), sizeof(price));
    out.write(reinterpret_cast<const char*>(&quantity), sizeof(quantity));
}

void Book::deserialize(std::ifstream& in) {
    size_t isbnLen, nameLen, authLen, keyLen;

    in.read(reinterpret_cast<char*>(&isbnLen), sizeof(isbnLen));
    ISBN.resize(isbnLen);
    in.read(&ISBN[0], isbnLen);

    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    bookName.resize(nameLen);
    in.read(&bookName[0], nameLen);

    in.read(reinterpret_cast<char*>(&authLen), sizeof(authLen));
    author.resize(authLen);
    in.read(&author[0], authLen);

    in.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
    keyword.resize(keyLen);
    in.read(&keyword[0], keyLen);

    in.read(reinterpret_cast<char*>(&price), sizeof(price));
    in.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));
}

BookManager::BookManager(const std::string& filename) : dataFile(filename) {
    loadFromFile();
}

BookManager::~BookManager() {
    saveToFile();
}

int BookManager::findBook(const std::string& ISBN) const {
    for (size_t i = 0; i < books.size(); ++i) {
        if (books[i].getISBN() == ISBN) {
            return i;
        }
    }
    return -1;
}

void BookManager::saveToFile() {
    std::ofstream out(dataFile, std::ios::binary);
    if (!out) return;

    size_t count = books.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& book : books) {
        book.serialize(out);
    }
    out.close();
}

void BookManager::loadFromFile() {
    std::ifstream in(dataFile, std::ios::binary);
    if (!in) return;

    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    books.resize(count);
    for (size_t i = 0; i < count; ++i) {
        books[i].deserialize(in);
    }
    in.close();
}

bool BookManager::addBook(const Book& book) {
    if (findBook(book.getISBN()) != -1) return false;
    if (!book.isValidISBN(book.getISBN()) || !book.isValidPrice(book.getPrice()) ||
        !book.isValidQuantity(book.getQuantity())) {
        return false;
    }

    // Only validate non-empty fields
    if (!book.getBookName().empty() && !book.isValidBookName(book.getBookName())) return false;
    if (!book.getAuthor().empty() && !book.isValidAuthor(book.getAuthor())) return false;
    if (!book.getKeyword().empty() && !book.isValidKeyword(book.getKeyword())) return false;

    books.push_back(book);
    std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
        return a.getISBN() < b.getISBN();
    });
    saveToFile();
    return true;
}

bool BookManager::selectBook(const std::string& ISBN) {
    int index = findBook(ISBN);
    if (index != -1) {
        selectedISBN = ISBN;
        return true;
    }

    Book newBook;
    newBook.setISBN(ISBN);
    if (addBook(newBook)) {
        selectedISBN = ISBN;
        return true;
    }
    return false;
}

bool BookManager::modifyBook(const std::string& ISBN, const std::string& bookName,
                            const std::string& author, const std::string& keyword, double price) {
    if (selectedISBN.empty()) return false;

    int index = findBook(selectedISBN);
    if (index == -1) return false;

    Book& book = books[index];

    if (!ISBN.empty() && ISBN != selectedISBN) {
        if (findBook(ISBN) != -1) return false;
        book.setISBN(ISBN);
    }
    if (!bookName.empty()) {
        if (!book.isValidBookName(bookName)) return false;
        book.setBookName(bookName);
    }
    if (!author.empty()) {
        if (!book.isValidAuthor(author)) return false;
        book.setAuthor(author);
    }
    if (!keyword.empty()) {
        if (!book.isValidKeyword(keyword)) return false;
        book.setKeyword(keyword);
    }
    if (price >= 0) {
        if (!book.isValidPrice(price)) return false;
        book.setPrice(price);
    }

    if (!ISBN.empty() && ISBN != selectedISBN) {
        std::sort(books.begin(), books.end(), [](const Book& a, const Book& b) {
            return a.getISBN() < b.getISBN();
        });
        selectedISBN = ISBN;
    }

    saveToFile();
    return true;
}

bool BookManager::importBook(int quantity, double totalCost) {
    if (selectedISBN.empty()) return false;
    if (quantity <= 0 || totalCost <= 0) return false;

    int index = findBook(selectedISBN);
    if (index == -1) return false;

    Book& book = books[index];
    book.setQuantity(book.getQuantity() + quantity);
    saveToFile();
    return true;
}

bool BookManager::buyBook(const std::string& ISBN, int quantity, double& totalPrice) {
    if (quantity <= 0) return false;

    int index = findBook(ISBN);
    if (index == -1) return false;

    Book& book = books[index];
    if (book.getQuantity() < quantity) return false;

    totalPrice = book.getPrice() * quantity;
    book.setQuantity(book.getQuantity() - quantity);
    saveToFile();
    return true;
}

std::vector<Book> BookManager::showBooks(const std::string& ISBN,
                                        const std::string& bookName,
                                        const std::string& author,
                                        const std::string& keyword) const {
    std::vector<Book> result;

    for (const auto& book : books) {
        bool match = true;

        if (!ISBN.empty() && book.getISBN() != ISBN) match = false;
        if (!bookName.empty() && book.getBookName() != bookName) match = false;
        if (!author.empty() && book.getAuthor() != author) match = false;
        if (!keyword.empty() && !book.hasKeyword(keyword)) match = false;

        if (match) {
            result.push_back(book);
        }
    }

    return result;
}