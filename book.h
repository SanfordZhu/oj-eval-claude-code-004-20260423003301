#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>
#include <fstream>
#include <set>

class Book {
private:
    std::string ISBN;
    std::string bookName;
    std::string author;
    std::string keyword;
    double price;
    int quantity;

public:
    Book();
    Book(const std::string& isbn, const std::string& name, const std::string& auth,
         const std::string& key, double pr, int qty);

    const std::string& getISBN() const { return ISBN; }
    const std::string& getBookName() const { return bookName; }
    const std::string& getAuthor() const { return author; }
    const std::string& getKeyword() const { return keyword; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void setISBN(const std::string& isbn) { ISBN = isbn; }
    void setBookName(const std::string& name) { bookName = name; }
    void setAuthor(const std::string& auth) { author = auth; }
    void setKeyword(const std::string& key) { keyword = key; }
    void setPrice(double pr) { price = pr; }
    void setQuantity(int qty) { quantity = qty; }

    bool isValidISBN(const std::string& isbn) const;
    bool isValidBookName(const std::string& name) const;
    bool isValidAuthor(const std::string& auth) const;
    bool isValidKeyword(const std::string& key) const;
    bool isValidPrice(double pr) const;
    bool isValidQuantity(int qty) const;

    std::vector<std::string> getKeywords() const;
    bool hasKeyword(const std::string& key) const;

    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);
};

class BookManager {
private:
    std::vector<Book> books;
    std::string selectedISBN;
    std::string dataFile;

    int findBook(const std::string& ISBN) const;
    void saveToFile();
    void loadFromFile();

public:
    BookManager(const std::string& filename = "books.dat");
    ~BookManager();

    bool addBook(const Book& book);
    bool selectBook(const std::string& ISBN);
    bool modifyBook(const std::string& ISBN, const std::string& bookName,
                   const std::string& author, const std::string& keyword, double price);
    bool importBook(int quantity, double totalCost);
    bool buyBook(const std::string& ISBN, int quantity, double& totalPrice);

    std::vector<Book> showBooks(const std::string& ISBN = "",
                               const std::string& bookName = "",
                               const std::string& author = "",
                               const std::string& keyword = "") const;

    const std::string& getSelectedISBN() const { return selectedISBN; }
    bool isBookSelected() const { return !selectedISBN.empty(); }
    void clearSelection() { selectedISBN.clear(); }
};

#endif