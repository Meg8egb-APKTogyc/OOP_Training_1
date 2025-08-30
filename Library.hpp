#pragma once
#include <chrono>
#include <ctime>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Books.hpp"
#include "Sequence/Sequence.hpp"
#include "Users.hpp"

enum class UserType { STUDENT, FACULTY, GUEST };

class BorrowingRecord {
 private:
  std::string userId;
  std::string bookId;
  std::chrono::steady_clock::time_point borrowDate;
  std::chrono::steady_clock::time_point dueDate;
  bool returned;

 public:
  BorrowingRecord(const std::string& userId, const std::string& bookId,
                  int borrowDays = 14)
      : userId(userId), bookId(bookId), returned(false) {
    borrowDate = std::chrono::steady_clock::now();
    dueDate = borrowDate + std::chrono::hours(24 * borrowDays);
  }

  ~BorrowingRecord() {}

  std::string getUserId() const { return userId; }
  std::string getBookId() const { return bookId; }

  std::string getBorrowDateStr() const {
    auto systemTime = std::chrono::system_clock::now() +
                      (borrowDate - std::chrono::steady_clock::now());
    std::time_t time = std::chrono::system_clock::to_time_t(systemTime);

    char buffer[26];
    ctime_r(&time, buffer);
    buffer[24] = '\0';
    return std::string(buffer);
  }

  std::string getDueDateStr() const {
    auto systemTime = std::chrono::system_clock::now() +
                      (dueDate - std::chrono::steady_clock::now());
    std::time_t time = std::chrono::system_clock::to_time_t(systemTime);

    char buffer[26];
    ctime_r(&time, buffer);
    buffer[24] = '\0';
    return std::string(buffer);
  }

  std::chrono::steady_clock::time_point getDueDate() const { return dueDate; }

  bool isReturned() const { return returned; }
  void markReturned() { returned = true; }

  static std::chrono::steady_clock::time_point now() {
    return std::chrono::steady_clock::now();
  }

  bool isOverdue() const {
    return !returned && std::chrono::steady_clock::now() > dueDate;
  }

  int getDaysOverdue() const {
    if (!isOverdue()) return 0;

    auto now = std::chrono::steady_clock::now();
    auto overdueDuration = now - dueDate;
    return std::chrono::duration_cast<std::chrono::hours>(overdueDuration)
               .count() /
           24;
  }
};

struct SearchResults {
  Sequence<Book>* byAuthor;
  Sequence<Book>* byTitle;
  Sequence<Book>* byGenre;
  Sequence<Book>* byISBN;

  SearchResults()
      : byAuthor(new MutableListSequence<Book>()),
        byTitle(new MutableListSequence<Book>()),
        byGenre(new MutableListSequence<Book>()),
        byISBN(new MutableListSequence<Book>()) {}

  ~SearchResults() {
    delete byAuthor;
    delete byTitle;
    delete byGenre;
    delete byISBN;
  }

  size_t totalCount() const {
    return byAuthor->GetLength() + byTitle->GetLength() + byGenre->GetLength() +
           byISBN->GetLength();
  }

  bool isEmpty() const {
    return byAuthor->GetLength() == 0 && byTitle->GetLength() == 0 &&
           byGenre->GetLength() == 0 && byISBN->GetLength() == 0;
  }
};

class LibraryOperations {
 public:
  virtual bool addBook(const std::string& title, const std::string& author,
                       const std::string& isbn, const std::string& genre) = 0;
  virtual bool removeBook(const std::string& isbn) = 0;
  virtual Book findBook(const std::string& isbn) = 0;
  virtual SearchResults* searchBooks(const std::string& query) = 0;
  virtual Sequence<Book>* getAllBooks() = 0;

  virtual bool registerUser(const std::string& name, const std::string& userId,
                            const std::string& email, UserType type) = 0;
  virtual bool registerUser(LibraryUser* user) = 0;
  virtual bool removeUser(const std::string& userId) = 0;
  virtual LibraryUser* findUser(const std::string& userId) = 0;
  virtual Sequence<LibraryUser*>* getAllUsers() = 0;

  virtual bool borrowBook(const std::string& userId,
                          const std::string& isbn) = 0;
  virtual bool returnBook(const std::string& userId,
                          const std::string& isbn) = 0;
  virtual Sequence<BorrowingRecord>* getOverdueBooks() = 0;
  virtual Sequence<BorrowingRecord>* getBorrowHistory() = 0;

  virtual ~LibraryOperations() {};
};

class Library : public LibraryOperations {
 private:
  std::unordered_map<std::string, Book>* books;
  std::unordered_map<std::string, LibraryUser*>* users;

  Sequence<BorrowingRecord>* borrowHistory;

  // Приблуды для поиска

  std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
  }

  bool containsWord(const std::string& text, const std::string& word) {
    std::string lowerText = toLower(text);
    std::string lowerWord = toLower(word);
    return lowerText.find(lowerWord) != std::string::npos;
  }

  bool exactMatch(const std::string& text, const std::string& word) {
    return toLower(text) == toLower(word);
  }

 public:
  Library()
      : books(new std::unordered_map<std::string, Book>()),
        users(new std::unordered_map<std::string, LibraryUser*>()),
        borrowHistory(new MutableListSequence<BorrowingRecord>()) {}

  ~Library() {
    delete books;
    delete users;
    delete borrowHistory;
  }

  Library(std::unordered_map<std::string, Book>* books,
          std::unordered_map<std::string, LibraryUser*>* users) {
    this->books = books;
    this->users = users;
  }

  // поиск по запросу

  SearchResults* searchBooks(const std::string& query) {
    SearchResults* results = new SearchResults();

    if (query.empty()) {
      return results;
    }

    std::string lowerQuery = toLower(query);

    for (const auto& pair : *books) {
      const Book& book = pair.second;

      if (exactMatch(book.getISBN(), lowerQuery)) {
        results->byISBN->Append(book);
        continue;
      }

      if (containsWord(book.getAuthor(), lowerQuery))
        results->byAuthor->Append(book);

      if (containsWord(book.getTitle(), lowerQuery))
        results->byTitle->Append(book);

      if (containsWord(book.getGenre(), lowerQuery))
        results->byGenre->Append(book);
    }

    return results;
  }

  virtual Book findBook(const std::string& isbn) override {
    if (books->find(isbn) == books->end()) return Book();

    return books->at(isbn);
  }

  virtual Sequence<Book>* getAllBooks() override {
    Sequence<Book>* allBooks = new MutableListSequence<Book>();
    for (auto& [key, val] : *books) {
      allBooks->Append(val);
    }
    return allBooks;
  }

  // Операции по книгами

  virtual bool addBook(const std::string& title, const std::string& author,
                       const std::string& isbn,
                       const std::string& genre) override {
    if (books->find(isbn) != books->end()) return false;

    books->insert({isbn, Book(title, author, genre, isbn)});
    return true;
  }

  virtual bool removeBook(const std::string& isbn) override {
    if (books->find(isbn) == books->end()) return false;

    books->erase(isbn);
    return true;
  }

  // Операции по пользователями

  virtual bool registerUser(const std::string& name, const std::string& userId,
                            const std::string& email, UserType type) override {
    if (users->find(userId) != users->end()) return false;

    LibraryUser* user = nullptr;
    switch (type) {
      case UserType::STUDENT:
        user = new Student(name, userId, email);
        break;
      case UserType::FACULTY:
        user = new Faculty(name, userId, email);
        break;
      case UserType::GUEST:
        user = new Guest(name, userId, email);
        break;
      default:
        return false;
    }

    users->insert({userId, user});
    return true;
  }

  virtual bool registerUser(LibraryUser* user) override {
    if (users->find(user->getUserId()) != users->end()) return false;

    users->insert({user->getUserId(), user});
    return true;
  }

  virtual bool removeUser(const std::string& userId) override {
    if (users->find(userId) == users->end()) return false;

    users->erase(userId);
    return true;
  }

  virtual LibraryUser* findUser(const std::string& userId) override {
    if (users->find(userId) == users->end()) return nullptr;

    return users->at(userId);
  }

  virtual Sequence<LibraryUser*>* getAllUsers() override {
    Sequence<LibraryUser*>* allUsers = new MutableListSequence<LibraryUser*>();
    for (auto& [key, val] : *users) {
      allUsers->Append(val);
    }
    return allUsers;
  }

  // Операции в билиблиотеке

  virtual bool borrowBook(const std::string& userId,
                          const std::string& isbn) override {
    if (users->find(userId) == users->end() ||
        books->find(isbn) == books->end())
      return false;

    LibraryUser* user = users->at(userId);
    Book& book = books->at(isbn);
    if (!user->canBorrow() || !book.isAvailable()) return false;

    book.setAvailable(false);
    user->getBorrowedBooks()->insert(isbn);
    borrowHistory->Append(BorrowingRecord(userId, isbn, user->getBorrowDays()));
    return true;
  }

  virtual bool returnBook(const std::string& userId,
                          const std::string& isbn) override {
    if (users->find(userId) == users->end() ||
        books->find(isbn) == books->end())
      return false;

    LibraryUser* user = users->at(userId);
    Book& book = books->at(isbn);
    if (user->getBorrowedBooks()->find(isbn) == user->getBorrowedBooks()->end())
      return false;

    book.setAvailable(true);
    user->getBorrowedBooks()->erase(isbn);
    for (BorrowingRecord& record : *borrowHistory) {
      if (record.getUserId() == userId && record.getBookId() == isbn) {
        record.markReturned();
        break;
      }
    }

    return true;
  }

  virtual Sequence<BorrowingRecord>* getOverdueBooks() override {
    Sequence<BorrowingRecord>* overdue =
        new MutableListSequence<BorrowingRecord>();
    for (BorrowingRecord& record : *borrowHistory) {
      if (record.isOverdue() && !record.isReturned()) {
        overdue->Append(record);
      }
    }
    return overdue;
  }

  virtual Sequence<BorrowingRecord>* getBorrowHistory() override {
    return borrowHistory;
  }
};
