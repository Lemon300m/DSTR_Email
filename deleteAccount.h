#ifndef DELETEACCOUNT_H
#define DELETEACCOUNT_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

inline void deleteAccount(const string& userEmail, const string& filePath) {
    if (userEmail.empty()) {
        cout << "No user is currently logged in.\n";
        return;
    }

    string confirmation;
    cout << "Are you sure you want to delete your account? (yes/no): ";
    cin >> confirmation;

    if (confirmation != "yes") {
        cout << "Account deletion cancelled.\n";
        return;
    }

    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error opening file.\n";
        return;
    }

    ofstream tempFile("temp.txt");
    string line;
    bool userFound = false;

    // Read each line and copy all but the target user's line to temp file
    while (getline(file, line)) {
        if (line.find(userEmail) == string::npos) {
            tempFile << line << '\n';
        } else {
            userFound = true;
        }
    }

    file.close();
    tempFile.close();

    // Replace the original file with the updated temp file if user was found
    if (userFound) {
        if (remove(filePath.c_str()) != 0 || rename("temp.txt", filePath.c_str()) != 0) {
            cerr << "Error updating user file.\n";
        } else {
            cout << "Account deleted successfully.\n";
        }
    } else {
        cout << "User not found in the file.\n";
        remove("temp.txt");  // Clean up if no user was found
    }
}

#endif
