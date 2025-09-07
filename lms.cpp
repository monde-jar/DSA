#include <iostream>
#include <string>
#include <fstream>

const int MAX_BOOKS = 100;
const int MAX_USERS = 10;

class Book {
private:
    std::string title;
    std::string author;
    std::string ISBN;
    bool isAvailable;

public:
    Book() : title(""), author(""), ISBN(""), isAvailable(true) {}
    
    Book(const std::string& title, const std::string& author, const std::string& ISBN, bool isAvailable = true)
        : title(title), author(author), ISBN(ISBN), isAvailable(isAvailable) {}

    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getISBN() const { return ISBN; }
    bool getAvailability() const { return isAvailable; }

    void setTitle(const std::string& newTitle) { title = newTitle; }
    void setAuthor(const std::string& newAuthor) { author = newAuthor; }
    void setISBN(const std::string& newISBN) { ISBN = newISBN; }
    void setAvailability(bool availability) { isAvailable = availability; }

    void updateAvailability(bool status) { isAvailable = status; }

    void display() const {
        std::cout << "Title: " << title << "\nAuthor: " << author 
                  << "\nISBN: " << ISBN << "\nStatus: " 
                  << (isAvailable ? "Available" : "Borrowed") << "\n\n";
    }

    std::string toString() const {
        return title + "|" + author + "|" + ISBN + "|" + (isAvailable ? "1" : "0");
    }

    static Book fromString(const std::string& data) {
        size_t pos1 = data.find('|');
        size_t pos2 = data.find('|', pos1 + 1);
        size_t pos3 = data.find('|', pos2 + 1);
        
        std::string title = data.substr(0, pos1);
        std::string author = data.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string ISBN = data.substr(pos2 + 1, pos3 - pos2 - 1);
        std::string availableStr = data.substr(pos3 + 1);
        
        bool available = (availableStr == "1");
        return Book(title, author, ISBN, available);
    }
};

class LibraryUser {
private:
    std::string userID;
    std::string name;
    std::string borrowedBooks[MAX_BOOKS];
    int borrowedCount;

public:
    LibraryUser() : userID(""), name(""), borrowedCount(0) {}
    
    LibraryUser(const std::string& userID, const std::string& name) 
        : userID(userID), name(name), borrowedCount(0) {}

    std::string getUserID() const { return userID; }
    std::string getName() const { return name; }
    int getBorrowedCount() const { return borrowedCount; }
    std::string getBorrowedBook(int index) const { return borrowedBooks[index]; }

    void setUserID(const std::string& newID) { userID = newID; }
    void setName(const std::string& newName) { name = newName; }

    bool borrowBook(const std::string& ISBN) {
        for (int i = 0; i < borrowedCount; i++) {
            if (borrowedBooks[i] == ISBN) {
                return false;
            }
        }
        if (borrowedCount < MAX_BOOKS) {
            borrowedBooks[borrowedCount] = ISBN;
            borrowedCount++;
            return true;
        }
        return false;
    }

    bool returnBook(const std::string& ISBN) {
        for (int i = 0; i < borrowedCount; i++) {
            if (borrowedBooks[i] == ISBN) {
                for (int j = i; j < borrowedCount - 1; j++) {
                    borrowedBooks[j] = borrowedBooks[j + 1];
                }
                borrowedCount--;
                return true;
            }
        }
        return false;
    }

    void displayBorrowedBooks() const {
        if (borrowedCount == 0) {
            std::cout << "No books borrowed.\n";
            return;
        }
        
        std::cout << "Books borrowed by " << name << " (ID: " << userID << "):\n";
        for (int i = 0; i < borrowedCount; i++) {
            std::cout << "- ISBN: " << borrowedBooks[i] << "\n";
        }
        std::cout << "\n";
    }

    std::string toString() const {
        std::string result = userID + "|" + name;
        for (int i = 0; i < borrowedCount; i++) {
            result += "|" + borrowedBooks[i];
        }
        return result;
    }

    static LibraryUser fromString(const std::string& data) {
        size_t pos1 = data.find('|');
        std::string userID = data.substr(0, pos1);
        size_t pos2 = data.find('|', pos1 + 1);
        std::string name = data.substr(pos1 + 1, pos2 - pos1 - 1);
        
        LibraryUser user(userID, name);
        
        size_t currentPos = pos2 + 1;
        while (currentPos < data.length()) {
            size_t nextPos = data.find('|', currentPos);
            if (nextPos == std::string::npos) {
                nextPos = data.length();
            }
            std::string isbn = data.substr(currentPos, nextPos - currentPos);
            user.borrowBook(isbn);
            currentPos = nextPos + 1;
        }
        
        return user;
    }
};

class Library {
private:
    Book* books[MAX_BOOKS];
    LibraryUser* users[MAX_USERS];
    int bookCount;
    int userCount;
    const std::string booksFile = "books.txt";
    const std::string usersFile = "users.txt";

public:
    Library() : bookCount(0), userCount(0) {
        for (int i = 0; i < MAX_BOOKS; i++) books[i] = nullptr;
        for (int i = 0; i < MAX_USERS; i++) users[i] = nullptr;
        loadBooksFromFile();
        loadUsersFromFile();
    }

    ~Library() {
        saveBooksToFile();
        saveUsersToFile();
        for (int i = 0; i < bookCount; i++) {
            delete books[i];
        }
        for (int i = 0; i < userCount; i++) {
            delete users[i];
        }
    }

    void loadBooksFromFile() {
        std::ifstream file(booksFile.c_str());
        if (!file.is_open()) {
            return;
        }

        std::string line;
        while (std::getline(file, line) && bookCount < MAX_BOOKS) {
            if (!line.empty()) {
                books[bookCount] = new Book(Book::fromString(line));
                bookCount++;
            }
        }
        file.close();
    }

    void loadUsersFromFile() {
        std::ifstream file(usersFile.c_str());
        if (!file.is_open()) {
            return;
        }

        std::string line;
        while (std::getline(file, line) && userCount < MAX_USERS) {
            if (!line.empty()) {
                users[userCount] = new LibraryUser(LibraryUser::fromString(line));
                userCount++;
            }
        }
        file.close();
    }

    void saveBooksToFile() {
        std::ofstream file(booksFile.c_str());
        if (!file.is_open()) {
            std::cerr << "Error: Could not save books to file.\n";
            return;
        }

        for (int i = 0; i < bookCount; i++) {
            file << books[i]->toString() << "\n";
        }
        file.close();
    }

    void saveUsersToFile() {
        std::ofstream file(usersFile.c_str());
        if (!file.is_open()) {
            std::cerr << "Error: Could not save users to file.\n";
            return;
        }

        for (int i = 0; i < userCount; i++) {
            file << users[i]->toString() << "\n";
        }
        file.close();
    }

    void addBook(const std::string& title, const std::string& author, const std::string& ISBN) {
        if (bookCount >= MAX_BOOKS) {
            std::cout << "Error: Maximum book capacity reached.\n";
            return;
        }
        
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getISBN() == ISBN) {
                std::cout << "Error: Book with ISBN " << ISBN << " already exists.\n";
                return;
            }
        }
        
        books[bookCount] = new Book(title, author, ISBN);
        bookCount++;
        std::cout << "Book added successfully.\n";
    }

    void removeBook(const std::string& ISBN) {
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getISBN() == ISBN) {
                delete books[i];
                for (int j = i; j < bookCount - 1; j++) {
                    books[j] = books[j + 1];
                }
                bookCount--;
                books[bookCount] = nullptr;
                std::cout << "Book removed successfully.\n";
                
                for (int j = 0; j < userCount; j++) {
                    users[j]->returnBook(ISBN);
                }
                return;
            }
        }
        std::cout << "Error: Book with ISBN " << ISBN << " not found.\n";
    }

    void registerUser(const std::string& userID, const std::string& name) {
        if (userCount >= MAX_USERS) {
            std::cout << "Error: Maximum user capacity reached.\n";
            return;
        }
        
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                std::cout << "Error: User with ID " << userID << " already exists.\n";
                return;
            }
        }
        
        users[userCount] = new LibraryUser(userID, name);
        userCount++;
        std::cout << "User registered successfully.\n";
    }

    void removeUser(const std::string& userID) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                delete users[i];
                for (int j = i; j < userCount - 1; j++) {
                    users[j] = users[j + 1];
                }
                userCount--;
                users[userCount] = nullptr;
                std::cout << "User removed successfully.\n";
                return;
            }
        }
        std::cout << "Error: User with ID " << userID << " not found.\n";
    }

    void displayAllBooks() const {
        if (bookCount == 0) {
            std::cout << "No books in the library.\n";
            return;
        }
        
        std::cout << "All books in the library:\n";
        for (int i = 0; i < bookCount; i++) {
            books[i]->display();
        }
    }

    void displayAllUsers() const {
        if (userCount == 0) {
            std::cout << "No registered users.\n";
            return;
        }
        
        std::cout << "All registered users:\n";
        for (int i = 0; i < userCount; i++) {
            std::cout << "User ID: " << users[i]->getUserID() << "\nName: " << users[i]->getName() << "\n\n";
        }
    }

    void borrowBook(const std::string& ISBN, const std::string& userID) {
        Book* foundBook = nullptr;
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getISBN() == ISBN) {
                foundBook = books[i];
                break;
            }
        }
        
        if (foundBook == nullptr) {
            std::cout << "Error: Book with ISBN " << ISBN << " not found.\n";
            return;
        }
        
        if (!foundBook->getAvailability()) {
            std::cout << "Error: Book is already borrowed.\n";
            return;
        }
        
        LibraryUser* foundUser = nullptr;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                foundUser = users[i];
                break;
            }
        }
        
        if (foundUser == nullptr) {
            std::cout << "Error: User with ID " << userID << " not found.\n";
            return;
        }
        
        if (foundUser->borrowBook(ISBN)) {
            foundBook->setAvailability(false);
            std::cout << "Book borrowed successfully.\n";
        } else {
            std::cout << "Error: User already has this book.\n";
        }
    }

    void returnBook(const std::string& ISBN, const std::string& userID) {
        LibraryUser* foundUser = nullptr;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                foundUser = users[i];
                break;
            }
        }
        
        if (foundUser == nullptr) {
            std::cout << "Error: User with ID " << userID << " not found.\n";
            return;
        }
        
        if (!foundUser->returnBook(ISBN)) {
            std::cout << "Error: User doesn't have this book.\n";
            return;
        }
        
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getISBN() == ISBN) {
                books[i]->setAvailability(true);
                break;
            }
        }
        
        std::cout << "Book returned successfully.\n";
    }

    void displayBorrowedBooks(const std::string& userID) const {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                users[i]->displayBorrowedBooks();
                return;
            }
        }
        std::cout << "Error: User with ID " << userID << " not found.\n";
    }
};

void displayMenu() {
    std::cout << "\nLibrary Management System\n";
    std::cout << "1. Add Book\n";
    std::cout << "2. Remove Book\n";
    std::cout << "3. Register User\n";
    std::cout << "4. Remove User\n";
    std::cout << "5. Borrow Book\n";
    std::cout << "6. Return Book\n";
    std::cout << "7. Display All Books\n";
    std::cout << "8. Display All Users\n";
    std::cout << "9. Display Borrowed Books\n";
    std::cout << "10. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    Library library;
    int choice;
    
    do {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1: {
                std::string title, author, ISBN;
                std::cout << "Enter book title: ";
                std::getline(std::cin, title);
                std::cout << "Enter author: ";
                std::getline(std::cin, author);
                std::cout << "Enter ISBN: ";
                std::getline(std::cin, ISBN);
                library.addBook(title, author, ISBN);
                break;
            }
            case 2: {
                std::string ISBN;
                std::cout << "Enter ISBN of book to remove: ";
                std::getline(std::cin, ISBN);
                library.removeBook(ISBN);
                break;
            }
            case 3: {
                std::string userID, name;
                std::cout << "Enter user ID: ";
                std::getline(std::cin, userID);
                std::cout << "Enter user name: ";
                std::getline(std::cin, name);
                library.registerUser(userID, name);
                break;
            }
            case 4: {
                std::string userID;
                std::cout << "Enter user ID to remove: ";
                std::getline(std::cin, userID);
                library.removeUser(userID);
                break;
            }
            case 5: {
                std::string ISBN, userID;
                std::cout << "Enter ISBN of book to borrow: ";
                std::getline(std::cin, ISBN);
                std::cout << "Enter user ID: ";
                std::getline(std::cin, userID);
                library.borrowBook(ISBN, userID);
                break;
            }
            case 6: {
                std::string ISBN, userID;
                std::cout << "Enter ISBN of book to return: ";
                std::getline(std::cin, ISBN);
                std::cout << "Enter user ID: ";
                std::getline(std::cin, userID);
                library.returnBook(ISBN, userID);
                break;
            }
            case 7:
                library.displayAllBooks();
                break;
            case 8:
                library.displayAllUsers();
                break;
            case 9: {
                std::string userID;
                std::cout << "Enter user ID: ";
                std::getline(std::cin, userID);
                library.displayBorrowedBooks(userID);
                break;
            }
            case 10:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 10);
    
    return 0;
}
