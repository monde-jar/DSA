#include <iostream>
#include <string>
#include <fstream>
using namespace std;
const int MAX_BOOKS = 10;
const int MAX_USERS = 10;

void clearScreen() {
    #ifdef _WIN32
        system("cls");//for windows
    #else
        system("clear");//for linux
    #endif
}

class Book {
private:
    string title;
    string author;
    string ISBN;
    bool isAvailable;

public:
    Book() : title(""), author(""), ISBN(""), isAvailable(true) {}
    
    Book(const string& title, const string& author, const string& ISBN, bool isAvailable = true)
        : title(title), author(author), ISBN(ISBN), isAvailable(isAvailable) {}

    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getISBN() const { return ISBN; }
    bool getAvailability() const { return isAvailable; }

    void setTitle(const string& newTitle) { title = newTitle; }
    void setAuthor(const string& newAuthor) { author = newAuthor; }
    void setISBN(const string& newISBN) { ISBN = newISBN; }
    void setAvailability(bool availability) { isAvailable = availability; }

    void updateAvailability(bool status) { isAvailable = status; }

    void display() const {
        cout << "Title: " << title << "\nAuthor: " << author 
                  << "\nISBN: " << ISBN << "\nStatus: " 
                  << (isAvailable ? "Available" : "Borrowed") << "\n\n";
    }

    string toString() const {
        return title + "|" + author + "|" + ISBN + "|" + (isAvailable ? "1" : "0");
    }

    static Book fromString(const string& data) {
        size_t pos1 = data.find('|');
        size_t pos2 = data.find('|', pos1 + 1);
        size_t pos3 = data.find('|', pos2 + 1);
        
        string title = data.substr(0, pos1);
        string author = data.substr(pos1 + 1, pos2 - pos1 - 1);
        string ISBN = data.substr(pos2 + 1, pos3 - pos2 - 1);
        string availableStr = data.substr(pos3 + 1);
        
        bool available = (availableStr == "1");
        return Book(title, author, ISBN, available);
    }
};

class LibraryUser {
private:
    string userID;
    string name;
    string borrowedBooks[MAX_BOOKS];
    int borrowedCount;

public:
    LibraryUser() : userID(""), name(""), borrowedCount(0) {}
    
    LibraryUser(const string& userID, const string& name) 
        : userID(userID), name(name), borrowedCount(0) {}

    string getUserID() const { return userID; }
    string getName() const { return name; }
    int getBorrowedCount() const { return borrowedCount; }
    string getBorrowedBook(int index) const { return borrowedBooks[index]; }

    void setUserID(const string& newID) { userID = newID; }
    void setName(const string& newName) { name = newName; }

    bool borrowBook(const string& ISBN) {
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

    bool returnBook(const string& ISBN) {
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
            cout << "No books borrowed.\n";
            return;
        }
        
        cout << "Books borrowed by " << name << " (ID: " << userID << "):\n";
        for (int i = 0; i < borrowedCount; i++) {
            cout << "- ISBN: " << borrowedBooks[i] << "\n";
        }
        cout << "\n";
    }

    string toString() const {
        string result = userID + "|" + name;
        for (int i = 0; i < borrowedCount; i++) {
            result += "|" + borrowedBooks[i];
        }
        return result;
    }

    static LibraryUser fromString(const string& data) {
        size_t pos1 = data.find('|');
        string userID = data.substr(0, pos1);
        size_t pos2 = data.find('|', pos1 + 1);
        string name = data.substr(pos1 + 1, pos2 - pos1 - 1);
        
        LibraryUser user(userID, name);
        
        size_t currentPos = pos2 + 1;
        while (currentPos < data.length()) {
            size_t nextPos = data.find('|', currentPos);
            if (nextPos == string::npos) {
                nextPos = data.length();
            }
            string isbn = data.substr(currentPos, nextPos - currentPos);
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
    const string booksFile = "books.txt";
    const string usersFile = "users.txt";

public:
    Library() : bookCount(0), userCount(0) {
        for (int i = 0; i < MAX_BOOKS; i++) books[i] = nullptr;
        for (int i = 0; i < MAX_USERS; i++) users[i] = nullptr;
        loadBooksFromFile();
        loadUsersFromFile();
    }

    ~Library() {
        saveBooksToFile();//as the word says
        saveUsersToFile();//this too
        for (int i = 0; i < bookCount; i++) {
            delete books[i];
        }
        for (int i = 0; i < userCount; i++) {
            delete users[i];
        }
    }

    void loadBooksFromFile() {
        ifstream file(booksFile.c_str());
        if (!file.is_open()) {
            return;
        }

        string line;
        while (getline(file, line) && bookCount < MAX_BOOKS) {
            if (!line.empty()) {
                books[bookCount] = new Book(Book::fromString(line));
                bookCount++;
            }
        }
        file.close();
    }

    void loadUsersFromFile() {
        ifstream file(usersFile.c_str());
        if (!file.is_open()) {
            return;
        }

        string line;
        while (getline(file, line) && userCount < MAX_USERS) {
            if (!line.empty()) {
                users[userCount] = new LibraryUser(LibraryUser::fromString(line));
                userCount++;
            }
        }
        file.close();
    }

    void saveBooksToFile() {
        ofstream file(booksFile.c_str());//opens da file
        if (!file.is_open()) {
            cerr << "Error: Could not save books to file.\n";
            return;
        }

        for (int i = 0; i < bookCount; i++) {
            file << books[i]->toString() << "\n";//writen the files
        }
        file.close();//then saved
    }

    void saveUsersToFile() {
        ofstream file(usersFile.c_str());//same lang
        if (!file.is_open()) {
            cerr << "Error: Could not save users to file.\n";
            return;
        }

        for (int i = 0; i < userCount; i++) {
            file << users[i]->toString() << "\n";//ganon din
        }
        file.close();//this too
    }

    void addBook(const string& title, const string& author, const string& ISBN) {
        if (bookCount >= MAX_BOOKS) {
            cout << "Error: Maximum book capacity reached.\n";
            return;
        }
        
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getISBN() == ISBN) {
                cout << "Error: Book with ISBN " << ISBN << " already exists.\n";
                return;
            }
        }
        
        books[bookCount] = new Book(title, author, ISBN);
        bookCount++;
        cout << "Book added successfully.\n";
    }

    void removeBook(const string& ISBN) {
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getISBN() == ISBN) {
                delete books[i];
                for (int j = i; j < bookCount - 1; j++) {
                    books[j] = books[j + 1];
                }
                bookCount--;
                books[bookCount] = nullptr;
                cout << "Book removed successfully.\n";
                
                for (int j = 0; j < userCount; j++) {
                    users[j]->returnBook(ISBN);
                }
                return;
            }
        }
        cout << "Error: Book with ISBN " << ISBN << " not found.\n";
    }

    void registerUser(const string& userID, const string& name) {
        if (userCount >= MAX_USERS) {
            cout << "Error: Maximum user capacity reached.\n";
            return;
        }
        
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                cout << "Error: User with ID " << userID << " already exists.\n";
                return;
            }
        }
        
        users[userCount] = new LibraryUser(userID, name);
        userCount++;
        cout << "User registered successfully.\n";
    }

    void removeUser(const string& userID) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                delete users[i];
                for (int j = i; j < userCount - 1; j++) {
                    users[j] = users[j + 1];
                }
                userCount--;
                users[userCount] = nullptr;
                cout << "User removed successfully.\n";
                return;
            }
        }
        cout << "Error: User with ID " << userID << " not found.\n";
    }

    void displayAllBooks() const {
        if (bookCount == 0) {
            cout << "No books in the library.\n";
            return;
        }
        
        cout << "All books in the library:\n";
        for (int i = 0; i < bookCount; i++) {
            books[i]->display();
        }
    }

    void displayAllUsers() const {
        if (userCount == 0) {
            cout << "No registered users.\n";
            return;
        }
        
        cout << "All registered users:\n";
        for (int i = 0; i < userCount; i++) {
            cout << "User ID: " << users[i]->getUserID() << "\nName: " << users[i]->getName() << "\n\n";
        }
    }

    void borrowBook(const string& ISBN, const string& userID) {
        Book* foundBook = nullptr;
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getISBN() == ISBN) {
                foundBook = books[i];
                break;
            }
        }
        
        if (foundBook == nullptr) {
            cout << "Error: Book with ISBN " << ISBN << " not found.\n";
            return;
        }
        
        if (!foundBook->getAvailability()) {
            cout << "Error: Book is already borrowed.\n";
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
            cout << "Error: User with ID " << userID << " not found.\n";
            return;
        }
        
        if (foundUser->borrowBook(ISBN)) {
            foundBook->setAvailability(false);
            cout << "Book borrowed successfully.\n";
        } else {
            cout << "Error: User already has this book.\n";
        }
    }

    void returnBook(const string& ISBN, const string& userID) {
        LibraryUser* foundUser = nullptr;
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                foundUser = users[i];
                break;
            }
        }
        
        if (foundUser == nullptr) {
            cout << "Error: User with ID " << userID << " not found.\n";
            return;
        }
        
        if (!foundUser->returnBook(ISBN)) {
            cout << "Error: User doesn't have this book.\n";
            return;
        }
        
        for (int i = 0; i < bookCount; i++) {
            if (books[i]->getISBN() == ISBN) {
                books[i]->setAvailability(true);
                break;
            }
        }
        
        cout << "Book returned successfully.\n";
    }

    void displayBorrowedBooks(const string& userID) const {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->getUserID() == userID) {
                users[i]->displayBorrowedBooks();
                return;
            }
        }
        cout << "Error: User with ID " << userID << " not found.\n";
    }
};

void displayMenu() {
    cout << "\n===============Library Management System===============\n";
    cout << "\t 1. Add Book\n";
    cout << "\t 2. Remove Book\n";
    cout << "\t 3. Register User\n";
    cout << "\t 4. Remove User\n";
    cout << "\t 5. Borrow Book\n";
    cout << "\t 6. Return Book\n";
    cout << "\t 7. Display All Books\n";
    cout << "\t 8. Display All Users\n";
    cout << "\t 9. Display Borrowed Books\n";
    cout << "\t 10. Clear Screen\n";
    cout << "\t 11. Exit\n";
    cout << "========================================================\n";
    cout << "Enter your choice: ";
}

int main() {
    Library library;
    int choice;
    
    do {
        displayMenu();
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1: {
                string title, author, ISBN;
                cout << "\nEnter book title: ";
                getline(cin, title);
                cout << "Enter author: ";
                getline(cin, author);
                cout << "Enter ISBN: ";
                getline(cin, ISBN);
                library.addBook(title, author, ISBN);
                break;
            }
            case 2: {
                string ISBN;
                cout << "\nEnter ISBN of book to remove: ";
                getline(cin, ISBN);
                library.removeBook(ISBN);
                break;
            }
            case 3: {
                string userID, name;
                cout << "\nEnter user ID: ";
                getline(cin, userID);
                cout << "Enter user name: ";
                getline(cin, name);
                library.registerUser(userID, name);
                break;
            }
            case 4: {
                string userID;
                cout << "\nEnter user ID to remove: ";
                getline(cin, userID);
                library.removeUser(userID);
                break;
            }
            case 5: {
                string ISBN, userID;
                cout << "\nEnter ISBN of book to borrow: ";
                getline(cin, ISBN);
                cout << "Enter user ID: ";
                getline(cin, userID);
                library.borrowBook(ISBN, userID);
                break;
            }
            case 6: {
                string ISBN, userID;
                cout << "\nEnter ISBN of book to return: ";
                getline(cin, ISBN);
                cout << "Enter user ID: ";
                getline(cin, userID);
                library.returnBook(ISBN, userID);
                break;
            }
            case 7:
                cout << "\n";
                library.displayAllBooks();
                break;
            case 8:
                cout << "\n";
                library.displayAllUsers();
                break;
            case 9: {
                string userID;
                cout << "\nEnter user ID: ";
                getline(cin, userID);
                library.displayBorrowedBooks(userID);
                break;
            }
            case 10:
                clearScreen();
                break;
            case 11:
                cout << "\nExiting...\n";
                break;
            default:
                cout << "\nInvalid choice. Please try again.\n";
        }
        
        // pause after each operation except clear screen then exit
        if (choice != 10 && choice != 11) {
            cout << "\nPress Enter to continue...";
            cin.get();
            clearScreen();
        }
    } while (choice != 11);
    
    return 0;
}
