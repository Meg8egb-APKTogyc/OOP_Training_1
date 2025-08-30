#pragma once
#include <string>
#include <unordered_set>

#include "Books.hpp"

class LibraryUser {
 protected:
  std::string name;
  std::string userId;
  std::string email;
  std::unordered_set<std::string>* borrowedBooks;

 public:
  LibraryUser()
      : name(""),
        userId(""),
        email(""),
        borrowedBooks(new std::unordered_set<std::string>()) {};

  ~LibraryUser() { delete borrowedBooks; }

  LibraryUser(std::string name, std::string userId, std::string email)
      : name(name),
        userId(userId),
        email(email),
        borrowedBooks(new std::unordered_set<std::string>()) {};

  virtual int getMaxBooks() const = 0;
  virtual int getBorrowDays() const = 0;

 public:
  bool canBorrow() { return borrowedBooks->size() < getMaxBooks(); }

  std::string getName() const { return name; }
  std::string getUserId() const { return userId; }
  std::string getEmail() const { return email; }
  std::unordered_set<std::string>* getBorrowedBooks() { return borrowedBooks; }

  void setName(std::string name) { this->name = name; }
  void setUserId(std::string userId) { this->userId = userId; }
  void setEmail(std::string email) { this->email = email; }
};

class Student : public LibraryUser {
 public:
  virtual int getMaxBooks() const override { return 3; }
  virtual int getBorrowDays() const override { return 14; }

  Student(std::string name, std::string userId, std::string email)
      : LibraryUser(name, userId, email) {}
};

class Faculty : public LibraryUser {
 public:
  virtual int getMaxBooks() const override { return 10; }
  virtual int getBorrowDays() const override { return 30; }

  Faculty(std::string name, std::string userId, std::string email)
      : LibraryUser(name, userId, email) {}
};

class Guest : public LibraryUser {
 public:
  virtual int getMaxBooks() const override { return 1; }
  virtual int getBorrowDays() const override { return 7; }

  Guest(std::string name, std::string userId, std::string email)
      : LibraryUser(name, userId, email) {}
};