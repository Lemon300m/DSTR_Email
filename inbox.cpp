#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

const int MAX_EMAILS = 100; // Maximum number of emails the inbox can hold

// Struct to store email details
struct Email {
    int id;
    string sender;
    string receiver;
    string subject;
    string content;
};

// Class to manage inbox operations
class Inbox {
private:
    Email emails[MAX_EMAILS];
    int emailCount;

public:
    Inbox() : emailCount(0) {}

    // Function to add a new email (used when reading from file)
    void addEmailFromFile(const string& sender, const string& receiver, const string& subject, const string& content) {
        if (emailCount < MAX_EMAILS) {
            emails[emailCount].id = emailCount + 1; // Assigning a unique ID to each email
            emails[emailCount].sender = sender;
            emails[emailCount].receiver = receiver;
            emails[emailCount].subject = subject;
            emails[emailCount].content = content;
            emailCount++;
        } else {
            cout << "Inbox is full! Cannot add more emails.\n";
        }
    }

    // Function to display all emails
    void displayEmails() const {
        if (emailCount == 0) {
            cout << "Inbox is empty.\n";
            return;
        }
        cout << "ID\tSender\t\tSubject\n";
        for (int i = 0; i < emailCount; i++) {
            cout << emails[i].id << "\t" << emails[i].sender << "\t\t" << emails[i].subject << "\n";
        }
    }

    // Function to view a specific email by ID
    void viewEmail(int id) const {
        for (int i = 0; i < emailCount; i++) {
            if (emails[i].id == id) {
                cout << "Sender: " << emails[i].sender << "\n";
                cout << "Subject: " << emails[i].subject << "\n";
                cout << "Content:\n" << emails[i].content << "\n";
                return;
            }
        }
        cout << "Email with ID " << id << " not found.\n";
    }

    // Function to delete an email by ID
    void deleteEmail(int id) {
        int index = -1;
        for (int i = 0; i < emailCount; i++) {
            if (emails[i].id == id) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            cout << "Email with ID " << id << " not found.\n";
            return;
        }

        // Shift emails to fill the gap
        for (int i = index; i < emailCount - 1; i++) {
            emails[i] = emails[i + 1];
        }
        emailCount--;
        cout << "Email deleted successfully!\n";
    }

    // Function to search emails by sender
    void searchBySender(const string& sender) const {
        bool found = false;
        for (int i = 0; i < emailCount; i++) {
            if (emails[i].sender == sender) {
                cout << "ID: " << emails[i].id << ", Subject: " << emails[i].subject << "\n";
                found = true;
            }
        }
        if (!found) {
            cout << "No emails found from " << sender << ".\n";
        }
    }

    // Function to load emails from a file
    void loadEmailsFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Failed to open the file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string sender, receiver, subject, content;

            // Read each field separated by a comma
            if (getline(ss, sender, ',') && getline(ss, receiver, ',') &&
                getline(ss, subject, ',') && getline(ss, content)) {
                addEmailFromFile(sender, receiver, subject, content);
            }
        }
        file.close();
        cout << "Emails loaded successfully from " << filename << ".\n";
    }
};

// Main function to demonstrate inbox functionality
int main() {
    Inbox myInbox;
    string filename = "emails.txt"; // Name of the file containing email data

    // Load emails from file
    myInbox.loadEmailsFromFile(filename);

    int choice;
    do {
        cout << "\nInbox Management System\n";
        cout << "1. Display All Emails\n";
        cout << "2. View Email by ID\n";
        cout << "3. Delete Email by ID\n";
        cout << "4. Search Email by Sender\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                myInbox.displayEmails();
                break;
            case 2: {
                int id;
                cout << "Enter email ID to view: ";
                cin >> id;
                myInbox.viewEmail(id);
                break;
            }
            case 3: {
                int id;
                cout << "Enter email ID to delete: ";
                cin >> id;
                myInbox.deleteEmail(id);
                break;
            }
            case 4: {
                cin.ignore(); // to handle input buffer
                string sender;
                cout << "Enter sender to search: ";
                getline(cin, sender);
                myInbox.searchBySender(sender);
                break;
            }
            case 0:
                cout << "Exiting the program.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 0);

    return 0;
}
