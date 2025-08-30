# KFD Test Assignment: Library Management System

## Task Description

Create a console-based library management system. This assignment tests your understanding of **OOP principles** and **data structures** - the foundation skills needed for our Kotlin course.

### What to Build
A working console application that manages:
- **Books** (add, remove, search)
- **Users** (different types: Student, Faculty, Guest) 
- **Borrowing operations** (borrow, return, track overdue)

### Business Rules
- **Student**: max 3 books, 14 days
- **Faculty**: max 10 books, 30 days  
- **Guest**: max 1 book, 7 days
- Books cannot be borrowed if unavailable
- Users cannot exceed their borrowing limit

## What We're Looking For

### ✅ **Does it work?** (Most Important)
- Program runs without crashes
- Basic features work (add book, borrow, return)
- Menu navigation functions

### ✅ **OOP Understanding**
- Different user types with different behavior
- Proper inheritance (User → Student/Faculty/Guest)
- Basic encapsulation (private fields, public methods)

### ✅ **Data Structure Choices**
- HashMap/Map for books and users (fast lookup)
- List for borrowing history (ordered records)
- Set for unique collections where appropriate
- Can explain your choices

### ✅ **Basic Programming Skills**
- Handles invalid input gracefully
- Code is readable and organized
- Reasonable variable/method names

## Required Features
1. Add/remove books
2. Register users (different types)
3. Borrow/return books with validation
4. Search books by title/author/ISBN
5. View overdue books

## Deliverables
1. **Working code** that demonstrates the features
2. **README** with setup instructions

**Time**: 1-2 days recommended

---

## Java Interface Examples

Use these as guidance - don't copy-paste, but understand the structure:

### Core Interfaces
```java
// What your library system should be able to do
public interface LibraryOperations {
    // Book management
    void addBook(String title, String author, String isbn, String genre);
    boolean removeBook(String isbn);
    Book findBook(String isbn);
    List<Book> searchBooks(String query);
    
    // User management
    void registerUser(String name, String userId, String email, UserType type);
    User findUser(String userId);
    
    // Borrowing operations
    boolean borrowBook(String userId, String isbn);
    boolean returnBook(String userId, String isbn);
    List<BorrowingRecord> getOverdueBooks();
}
```

### Basic Class Structure
```java
// Abstract User class - implement with inheritance
public abstract class User {
    protected String name;
    protected String userId;
    protected String email;
    protected List<String> borrowedBooks;
    
    public User(String name, String userId, String email) {
        this.name = name;
        this.userId = userId;
        this.email = email;
        this.borrowedBooks = new ArrayList<>();
    }
    
    // Each user type implements these differently
    public abstract int getMaxBooks();
    public abstract int getBorrowDays();
    public abstract double getFinePerDay();
    
    public boolean canBorrow() {
        return borrowedBooks.size() < getMaxBooks();
    }
    
    // Getters and setters...
}

// Example implementation
public class Student extends User {
    public Student(String name, String userId, String email) {
        super(name, userId, email);
    }
    
    @Override
    public int getMaxBooks() { return 3; }
    
    @Override
    public int getBorrowDays() { return 14; }
    
    @Override
    public double getFinePerDay() { return 0.50; }
}

// Faculty and Guest classes follow similar pattern...
```

### Data Structure Usage
```java
public class Library implements LibraryOperations {
    // Use HashMap for fast lookup by key
    private Map<String, Book> books;      // ISBN -> Book
    private Map<String, User> users;      // UserID -> User
    
    // Use List for ordered collections
    private List<BorrowingRecord> borrowingHistory;
    
    // Use Set for unique collections
    private Set<String> genres;
    
    public Library() {
        books = new HashMap<>();           // O(1) book lookup
        users = new HashMap<>();           // O(1) user lookup
        borrowingHistory = new ArrayList<>(); // Chronological order
        genres = new HashSet<>();          // Unique genres only
    }
    
    @Override
    public boolean borrowBook(String userId, String isbn) {
        User user = users.get(userId);     // Fast lookup
        Book book = books.get(isbn);       // Fast lookup
        
        // Validation logic
        if (user == null || book == null) return false;
        if (!book.isAvailable()) return false;
        if (!user.canBorrow()) return false;
        
        // Process borrowing
        book.setAvailable(false);
        user.getBorrowedBooks().add(isbn);
        borrowingHistory.add(new BorrowingRecord(user, book, LocalDate.now()));
        
        return true;
    }
    
    // Implement other methods...
}
```

### Simple Console Interface
```java
public class LibraryConsole {
    private Library library;
    private Scanner scanner;
    
    public void run() {
        while (true) {
            showMenu();
            int choice = getIntInput("Enter choice: ");
            
            switch (choice) {
                case 1: handleBookManagement(); break;
                case 2: handleUserManagement(); break;
                case 3: handleBorrowing(); break;
                case 0: return; // Exit
                default: System.out.println("Invalid choice");
            }
        }
    }
    
    private void showMenu() {
        System.out.println("\n=== Library Management ===");
        System.out.println("1. Book Management");
        System.out.println("2. User Management");
        System.out.println("3. Borrowing Operations");
        System.out.println("0. Exit");
    }
    
    private int getIntInput(String prompt) {
        while (true) {
            try {
                System.out.print(prompt);
                return Integer.parseInt(scanner.nextLine().trim());
            } catch (NumberFormatException e) {
                System.out.println("Please enter a valid number.");
            }
        }
    }
}
```

---

## Evaluation Notes

### Good Signs
- Program works for basic scenarios
- Clear inheritance structure  
- Appropriate HashMap/List usage
- Some error handling
- Readable code organization

### Red Flags
- Doesn't compile/run
- No inheritance used
- Only arrays, no collections
- No error handling
- Unreadable code, no linting

**Goal**: Find students who understand basics and can think through problems systematically. Perfect code not required - working code with reasonable design is what we want to see.
