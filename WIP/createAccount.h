#ifndef CREATEACCOUNT_H
#define CREATEACCOUNT_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Unreachable function 
static inline bool isValidEmailService(const string& service) {
    ifstream file("mail_service.txt");
    if (!file.is_open()) {
        cerr << "Error: Could not open the valid email services file.\n";
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (line == service) {
            return true;  // Found a match
        }
    }

    return false;  // No match found
}

inline void createAccount(const string& filePath) {
    string service, emailName, email, password, username;

    // Get email service and validate it
    bool valid = false;
    while (!valid) {
        cout << "Enter the email service (e.g., gmail.com, outlook.com): ";
        cin >> service;
        if (!isValidEmailService(service)) {
            cout << "Invalid email service." << endl;
        } else {
            valid = true;
        }
    }

    // Get the full email address
    cout << "Enter your email name (without @" << service << "): ";
    cin >> emailName;
    email = emailName + "@" + service;

    // Check if email is already in use
    ifstream file(filePath);
    string line, existingEmail;
    bool isAvailable = true;

    if (file.is_open()) {
        while (getline(file, line)) {
            size_t firstComma = line.find(',');
            if (firstComma != string::npos) {
                existingEmail = line.substr(0, firstComma);
                if (existingEmail == email) {
                    isAvailable = false;
                    break;
                }
            }
        }
        file.close();
    } else {
        cerr << "Error: Could not open file.\n";
        return;
    }

    if (!isAvailable) {
        cout << "Error: This email is already taken. Please try again.\n";
        return;
    }

    // Get password and username
    cout << "Enter your password: ";
    cin >> password;
    cout << "Enter your username: ";
    cin >> username;

    // Save the new user information to the file
    ofstream outFile(filePath, ios::app);
    if (outFile.is_open()) {
        outFile << email << ", " << password << ", " << username << "\n";
        outFile.close();
        cout << "User successfully registered! Please log in.\n";
    } else {
        cerr << "Error: Could not open file for writing.\n";
    }
}

#endif