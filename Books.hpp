#pragma once
#include <string>

class Book {
 private:
  std::string title;
  std::string author;
  std::string genre;
  std::string isbn;
  bool available;

 public:
  Book() : title(""), author(""), genre(""), isbn(""), available(true) {}

  ~Book() {}

  Book(std::string title, std::string author, std::string genre,
       std::string isbn)
      : title(title),
        author(author),
        genre(genre),
        isbn(isbn),
        available(true) {}

  Book(std::string title, std::string author, std::string genre,
       std::string isbn, bool isAvailable)
      : title(title),
        author(author),
        genre(genre),
        isbn(isbn),
        available(isAvailable) {}

  std::string getTitle() const { return title; }
  std::string getAuthor() const { return author; }
  std::string getGenre() const { return genre; }
  std::string getISBN() const { return isbn; }

  bool isAvailable() const { return available; }

  void setAvailable(bool val) { available = val; }
};
