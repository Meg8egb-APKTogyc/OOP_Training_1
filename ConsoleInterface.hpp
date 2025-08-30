#pragma once
#include <iomanip>
#include <iostream>

#include "Library.hpp"

class ConsoleInterface {
 private:
  Library library;

  void printMenu() {
    std::cout << "\n=== Library Management System ===\n";
    std::cout << "1. Add Book\n";
    std::cout << "2. Remove Book\n";
    std::cout << "3. Register User\n";
    std::cout << "4. Remove User\n";
    std::cout << "5. Borrow Book\n";
    std::cout << "6. Return Book\n";
    std::cout << "7. Search Books\n";
    std::cout << "8. View Overdue Books\n";
    std::cout << "9. View All Books\n";
    std::cout << "10. View All Users\n";
    std::cout << "11. View Borrowed Books History\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose option: ";
  }

  int getIntInput(const std::string& prompt) {
    int value;
    while (true) {
      std::cout << prompt;
      if (std::cin >> value) {
        std::cin.ignore();
        return value;
      } else {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
      }
    }
  }

  std::string getStringInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
  }

  UserType getUserType() {
    std::cout << "Select user type:\n";
    std::cout << "1. Student\n";
    std::cout << "2. Faculty\n";
    std::cout << "3. Guest\n";

    int choice;
    while (true) {
      choice = getIntInput("Enter choice (1-3): ");
      if (choice >= 1 && choice <= 3) break;
      std::cout << "Invalid choice. Please enter 1-3.\n";
    }

    switch (choice) {
      case 1:
        return UserType::STUDENT;
      case 2:
        return UserType::FACULTY;
      case 3:
        return UserType::GUEST;
      default:
        return UserType::STUDENT;
    }
  }

  void addBook() {
    std::cout << "\n--- Add Book ---\n";
    std::string title = getStringInput("Title: ");
    std::string author = getStringInput("Author: ");
    std::string isbn = getStringInput("ISBN: ");
    std::string genre = getStringInput("Genre: ");

    if (library.addBook(title, author, isbn, genre)) {
      std::cout << "Book added successfully!\n";
    } else {
      std::cout << "Failed to add book (ISBN might already exist).\n";
    }
  }

  void removeBook() {
    std::cout << "\n--- Remove Book ---\n";
    std::string isbn = getStringInput("Enter ISBN to remove: ");

    if (library.removeBook(isbn)) {
      std::cout << "Book removed successfully!\n";
    } else {
      std::cout << "Book not found.\n";
    }
  }

  void registerUser() {
    std::cout << "\n--- Register User ---\n";
    std::string name = getStringInput("Name: ");
    std::string userId = getStringInput("User ID: ");
    std::string email = getStringInput("Email: ");
    UserType type = getUserType();

    if (library.registerUser(name, userId, email, type)) {
      std::cout << "User registered successfully!\n";
    } else {
      std::cout << "Failed to register user (ID might already exist).\n";
    }
  }

  void removeUser() {
    std::cout << "\n--- Remove User ---\n";
    std::string userId = getStringInput("Enter User ID to remove: ");

    if (library.removeUser(userId)) {
      std::cout << "User removed successfully!\n";
    } else {
      std::cout << "User not found.\n";
    }
  }

  void borrowBook() {
    std::cout << "\n--- Borrow Book ---\n";
    std::string userId = getStringInput("User ID: ");
    std::string isbn = getStringInput("Book ISBN: ");

    if (library.borrowBook(userId, isbn)) {
      std::cout << "Book borrowed successfully!\n";
    } else {
      std::cout << "Failed to borrow book. Check if:\n";
      std::cout << "- User exists and can borrow more books\n";
      std::cout << "- Book exists and is available\n";
    }
  }

  void returnBook() {
    std::cout << "\n--- Return Book ---\n";
    std::string userId = getStringInput("User ID: ");
    std::string isbn = getStringInput("Book ISBN: ");

    if (library.returnBook(userId, isbn)) {
      std::cout << "Book returned successfully!\n";
    } else {
      std::cout << "Failed to return book. Check if user borrowed this book.\n";
    }
  }

  void printBook(const Book& book) {
    std::cout << "Title: " << book.getTitle() << "\n";
    std::cout << "Author: " << book.getAuthor() << "\n";
    std::cout << "ISBN: " << book.getISBN() << "\n";
    std::cout << "Genre: " << book.getGenre() << "\n";
    std::cout << "Available: " << (book.isAvailable() ? "Yes" : "No") << "\n";
    std::cout << "------------------------\n";
  }

  void searchBooks() {
    std::cout << "\n--- Search Books ---\n";
    std::string query = getStringInput("Enter search query: ");

    SearchResults* results = library.searchBooks(query);

    if (results->isEmpty()) {
      std::cout << "No books found.\n";
    } else {
      std::cout << "\n=== Search Results ===\n";

      if (results->byISBN->GetLength() > 0) {
        std::cout << "\nBy ISBN:\n";
        std::cout << "------------------------\n";
        for (const auto& book : *results->byISBN) {
          printBook(book);
        }
      }

      if (results->byAuthor->GetLength() > 0) {
        std::cout << "\nBy Author:\n";
        std::cout << "------------------------\n";
        for (const auto& book : *results->byAuthor) {
          printBook(book);
        }
      }

      if (results->byTitle->GetLength() > 0) {
        std::cout << "\nBy Title:\n";
        std::cout << "------------------------\n";
        for (const auto& book : *results->byTitle) {
          printBook(book);
        }
      }

      if (results->byGenre->GetLength() > 0) {
        std::cout << "\nBy Genre:\n";
        std::cout << "------------------------\n";
        for (const auto& book : *results->byGenre) {
          printBook(book);
        }
      }

      std::cout << "\n"
                << "Total found: " << results->totalCount() << " books\n";
    }

    delete results;
  }

  void viewOverdueBooks() {
    std::cout << "\n--- Overdue Books ---\n";
    Sequence<BorrowingRecord>* overdue = library.getOverdueBooks();

    if (overdue->GetLength() == 0) {
      std::cout << "No overdue books.\n";
    } else {
      std::cout << "Overdue books (" << overdue->GetLength() << "):\n";
      for (const auto& record : *overdue) {
        std::cout << "User: " << record.getUserId() << "\n";
        std::cout << "Book: " << record.getBookId() << "\n";
        std::cout << "Borrowed: " << record.getBorrowDateStr() << "\n";
        std::cout << "Due: " << record.getDueDateStr() << "\n";
        std::cout << "------------------------\n";
      }
    }

    delete overdue;
  }

  void viewAllBooks() {
    Sequence<Book>* allBooks = library.getAllBooks();

    std::cout << "--- All books (" << allBooks->GetLength() << "): ---\n";
    for (const auto& book : *allBooks) {
      printBook(book);
    }

    delete allBooks;
  }

  void viewAllUsers() {
    Sequence<LibraryUser*>* allUsers = library.getAllUsers();

    std::cout << "All users (" << allUsers->GetLength() << "):\n";
    for (const auto& user : *allUsers) {
      std::cout << "ID: " << user->getUserId() << "\n";
      std::cout << "Name: " << user->getName() << "\n";
      std::cout << "Email: " << user->getEmail() << "\n";

      std::unordered_set<std::string>* borrowed = user->getBorrowedBooks();
      if (borrowed->size() > 0) {
        std::cout << "Borrowed books (" << borrowed->size() << "):\n";
        for (const auto& bookId : *borrowed) {
          std::cout << "- " << bookId << "\n";
        }
      }

      std::cout << "------------------------\n";
    }

    delete allUsers;
  }

  void viewBorrowedBooksHistory() {
    std::cout << "Borrowed books history:\n";
    std::cout << "------------------------\n";
    for (const auto& record : *library.getBorrowHistory()) {
      std::cout << "User: " << record.getUserId() << "\n";
      std::cout << "Book: " << record.getBookId() << "\n";
      std::cout << "Borrowed: " << record.getBorrowDateStr() << "\n";
      std::cout << "Due: " << record.getDueDateStr() << "\n";
      std::cout << "Status: "
                << (record.isOverdue()    ? "OVERDUE"
                    : record.isReturned() ? "RETURNED"
                                          : "BORROWED")
                << "\n";
      std::cout << "------------------------\n";
    }
  }

 public:
  void run() {
    std::cout << "Welcome to Library Management System!\n";

    while (true) {
      printMenu();
      int choice = getIntInput("");

      switch (choice) {
        case 1:
          addBook();
          break;
        case 2:
          removeBook();
          break;
        case 3:
          registerUser();
          break;
        case 4:
          removeUser();
          break;
        case 5:
          borrowBook();
          break;
        case 6:
          returnBook();
          break;
        case 7:
          searchBooks();
          break;
        case 8:
          viewOverdueBooks();
          break;
        case 9:
          viewAllBooks();
          break;
        case 10:
          viewAllUsers();
          break;
        case 11:
          viewBorrowedBooksHistory();
          break;
        case 0:
          std::cout << "Goodbye!\n";
          return;
        default:
          std::cout << "Invalid choice. Please try again.\n";
      }

      std::cout << "\nPress Enter to continue...";
      std::cin.ignore();
    }
  }
};