#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <cstdlib>

#include "commonFunction.h"
#include "outbox.h"     //still in progress so will be commented out for now
#include "inbox.h"
#include "deleteAccount.h"
#include "createAccount.h"
#include "windows.h"

using namespace std;

class LoginSystem {
private:
    struct User {
        string email;
        string password;
        string name;
        User* next;
    };

    User* head;
    bool isLoggedIn;
    string filePath;
    string email, passwd, name;

    void loadUserData() {
        // WARNING This function will clear the existing linked list data
        while (head != nullptr) {
            User* temp = head;
            head = head->next;
            delete temp;
        }
        //==============================================

        ifstream file(filePath);
        if (!file.is_open()) {
            cerr << "Error opening file.\n";
            return;
        }

        string line, delimiter = "|||";
        while (getline(file, line)) {
            size_t pos = 0;
            string email, password, name;

            // Extract email
            if ((pos = line.find(delimiter)) != string::npos) {
                email = line.substr(0, pos);
                line.erase(0, pos + delimiter.length());
            } else {
                cerr << "Error reading line: " << line << "\n";
                continue;
            }

            // Extract password
            if ((pos = line.find(delimiter)) != string::npos) {
                password = line.substr(0, pos);
                line.erase(0, pos + delimiter.length());
            } else {
                cerr << "Error reading line: " << line << "\n";
                continue;
            }

            // The remaining part is the name
            name = line;

            email = trim(email);
            password = trim(password);
            name = trim(name);

            addUserToLinkedList(email, password, name);
        }
        file.close();
    }

    void addUserToLinkedList(const string& email, const string& password, const string& name) {
        User* newUser = new User{email, password, name, head};
        head = newUser;
    }

    User* findUserByEmail(const string& email) {
        User* current = head;
        while (current != nullptr) {
            if (current->email == email) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

public:
    struct UserDetails {
        string email;
        string passwd;
        string name;
    };

    LoginSystem() : head(nullptr), isLoggedIn(false), filePath("users.txt") {}

    // Destructor to free the linked list memory
    ~LoginSystem() {
        User* current = head;
        while (current != nullptr) {
            User* next = current->next;
            delete current;
            current = next;
        }
    }

    // Initializes the system with a user file and loads user data
    void initialize(const string& filePath) {
        this->filePath = filePath;
        loadUserData();
    }

    // Verifies the login credentials
    bool verifyLogin() {
        string filePath = "users.txt";
        initialize(filePath);
        cout << "Enter email: ";
        cin >> email;
        cout << "Enter password: ";
        cin >> passwd;

        User* user = findUserByEmail(email);
        if (user != nullptr && user->password == passwd) {
            isLoggedIn = true;
            email = user->email;
            passwd = user->password;
            name = user->name;
            return true;

        } else {
            cout << "Invalid email or password.\n";
            return false;
        }
    }
    
    UserDetails getDetail() const { return {email, passwd, name}; }

    // Checks if a user is currently logged in
    bool isUserLoggedIn() const {
        return isLoggedIn;
    }
    void logout() {
        isLoggedIn = false;
        email.clear();
        passwd.clear();
        name.clear();
        cout << "You are being logged out.\n";
        wait(1000);
        clearScreen();
        cout << "You have been logged out.\n";
    }
};

int main() {
    string time = getCurrentTimeAsString();
    LoginSystem system;
    string userfilePath = "users.txt";
    DoublyLinkedList inbox;
    inbox.loadEmailsFromFile("emails.txt");

    while (true) {
        int choice;
        bool valid_input = false;
        do {
            cout << time << endl;
            cout << R"(==============================================
Main Menu:
    1. Login
    2. Create new account
    0. Terminate Program
==============================================
Choose an option: )";
            cin >> choice;
            if (!(valid_input = !cin.fail())) { 
                clearScreen();
                cout << "That input is invalid!\n";
                cin.clear();
                cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            } else {
                switch (choice) {
                    case 0:
                        cout << "Terminating the program. See You Space Cowboy!\n";
                        wait(900);
                        exit(0);
                    case 1:
                        clearScreen();
                        if (system.verifyLogin()) {
                            clearScreen();
                            LoginSystem::UserDetails userDetails = system.getDetail();
                            cout << "Login successful. Welcome, " << userDetails.name << "!\n";
                            bool logOut = false;
                            int action;
                            while (!logOut) {
                                bool inner_valid_input = false;
                                do {
                                    cout << R"(==============================================
Available actions:
    1. Send an email
    2. Access Inbox
    3. Delete Account
    0. Logout
==============================================
Select an action: )";
                                    cin >> action;
                                    if (!(inner_valid_input = !cin.fail())) { 
                                        cout << "That input is invalid!\n";
                                        cin.clear();
                                        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
                                    } else {
                                        switch (action) {
                                            case 1: {
                                                Outbox outbox(userDetails.email);
                                                outboxMenu(outbox);
                                                break;  
                                            }
                                            case 2:
                                                inbox.inboxMenu(userDetails.email);
                                                break;
                                            case 3:
                                                deleteAccount(userDetails.email, userfilePath);
                                                logOut = true;
                                                system.logout();
                                                break;
                                            case 0:
                                                logOut = true;
                                                system.logout();
                                                break;
                                            default:
                                                cout << "Invalid Selection, please try again.\n";
                                                break;
                                        }
                                    }
                                } while (!inner_valid_input);
                            }
                        }
                        break;
                    case 2:
                        clearScreen();
                        createAccount(userfilePath);
                        wait(2000);
                        clearScreen();
                        break;
                    default:
                        cout << "Invalid Selection, please try again.\n";
                        break;
                }
            }
        } while (!valid_input);
    } 
    return 0;
}
