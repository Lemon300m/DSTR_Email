#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <cstdlib> //This is mainly just for the exit function

#include "performSearch.h"
#include "accessInbox.h"
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

        string line;
        while (getline(file, line)) {
            istringstream ss(line);
            string email, password, name;

            // Parse the line assuming the format: email, password, name
            if (getline(ss, email, ',') &&
                getline(ss, password, ',') &&
                getline(ss, name, ',')) {

                // Trim whitespace from parsed fields
                email = trim(email);
                password = trim(password);
                name = trim(name);

                addUserToLinkedList(email, password, name);
            } else {
                cerr << "Error reading line: " << line << "\n";
            }
        }
        file.close();
    }

    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return (first == string::npos) ? "" : str.substr(first, (last - first + 1));
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

    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void wait(int time){
        Sleep(time);
    }
};

int main() {
    LoginSystem system;
    string userfilePath = "users.txt";

    while (true) {
        int choice;
        bool valid_input = false;
        do {
            cout << R"(==============================================
Main Menu:
    1. Login
    2. Create new account
    0. Terminate Program
==============================================
Choose an option: )";
            cin >> choice;
            if (!(valid_input = !cin.fail())) { 
                cout << "That input is invalid!\n";
                cin.clear();
                cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            } else {
                switch (choice) {
                    case 0:
                        cout << "Terminating the program. See You Space Cowboy!\n";
                        system.wait(900);
                        exit(0);
                    case 1:
                        system.clearScreen();
                        if (system.verifyLogin()) {
                            system.clearScreen();
                            LoginSystem::UserDetails userDetails = system.getDetail();
                            cout << "Login successful. Welcome, " << userDetails.name << "!\n";
                            bool logOut = false;
                            int action;
                            while (!logOut) {
                                bool valid_input = false;
                                do {
                                    cout << R"(==============================================
Available actions:
    1. Perform Search
    2. Access Inbox
    3. Delete Account
    0. Logout
==============================================
Select an action: )";
                                    cin >> action;
                                    if (!(valid_input = !cin.fail())) { 
                                        cout << "That input is invalid!\n";
                                        cin.clear();
                                        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
                                    } else {
                                        switch (action) {
                                            case 1:
                                                performSearch();
                                                break;
                                            case 2:
                                                accessInbox();
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
                                } while (!valid_input);
                            }
                        }
                        break;
                    case 2:
                        system.clearScreen();
                        createAccount(userfilePath);
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
