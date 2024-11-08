#ifndef INBOX_H
#define INBOX_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

// Email structure
struct Email {
    int id;
    string sender;
    string receiver;
    string subject;
    string content;
};

// Node for the doubly linked list
struct Node {
    Email email;
    Node* prev;
    Node* next;

    Node(const Email& e) : email(e), prev(nullptr), next(nullptr) {}
};

// Doubly Linked List for Inbox Management
class DoublyLinkedList {
private:
    Node* head;
    Node* tail;
    int emailCount;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), emailCount(0) {}

    ~DoublyLinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    void addEmail(const Email& email) {
        Node* newNode = new Node(email);
        if (tail == nullptr) {  // List is empty
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        emailCount++;
    }

    void displayEmails() const {
        if (head == nullptr) {
            cout << "Inbox is empty.\n";
            return;
        }

        Node* current = head;
        cout << "ID\tSender\t\tSubject\n";
        while (current != nullptr) {
            const Email& email = current->email;
            cout << email.id << "\t" << email.sender << "\t\t" << email.subject << "\n";
            current = current->next;
        }
    }

    void viewEmail(int id) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->email.id == id) {
                const Email& email = current->email;
                cout << "Sender: " << email.sender << "\n";
                cout << "Receiver: " << email.receiver << "\n";
                cout << "Subject: " << email.subject << "\n";
                cout << "Content:\n" << email.content << "\n";
                return;
            }
            current = current->next;
        }
        cout << "Email with ID " << id << " not found.\n";
    }

    void deleteEmail(int id) {
        Node* current = head;
        while (current != nullptr) {
            if (current->email.id == id) {
                if (current->prev) {
                    current->prev->next = current->next;
                } else {
                    head = current->next;
                }
                if (current->next) {
                    current->next->prev = current->prev;
                } else {
                    tail = current->prev;
                }
                delete current;
                emailCount--;
                cout << "Email with ID " << id << " deleted.\n";
                return;
            }
            current = current->next;
        }
        cout << "Email with ID " << id << " not found.\n";
    }

    void searchBySender(const string& sender) const {
        Node* current = head;
        bool found = false;
        while (current != nullptr) {
            if (current->email.sender == sender) {
                const Email& email = current->email;
                cout << "ID: " << email.id << ", Subject: " << email.subject << "\n";
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "No emails found from sender: " << sender << "\n";
        }
    }

    void searchByTitle(const string& title) const {
        Node* current = head;
        bool found = false;
        string lowerTitle = toLower(title);
        while (current != nullptr) {
            string lowerSubject = toLower(current->email.subject);
            if (lowerSubject.find(lowerTitle) != string::npos) {
                const Email& email = current->email;
                cout << "ID: " << email.id << ", Sender: " << email.sender << ", Receiver: " << email.receiver << "\n";
                cout << "Content: " << email.content << "\n\n";
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "No emails found with title: " << title << "\n";
        }
    }

    void loadEmailsFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Failed to open the file: " << filename << "\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream emailStream(line);
            string sender, receiver, subject, content;
            if (getline(emailStream, sender, ',') &&
                getline(emailStream, receiver, ',') &&
                getline(emailStream, subject, ',') &&
                getline(emailStream, content)) {
                addEmail({emailCount + 1, sender, receiver, subject, content});
            }
        }
        file.close();
        cout << "Emails loaded successfully from " << filename << ".\n";
    }

    static string toLower(const string& str) {
        string lowerStr = str;
        transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        return lowerStr;
    }

    void inboxMenu() {
        int choice;
        do {
            cout << "\nInbox Management\n";
            cout << "1. View All Received Emails\n";
            cout << "2. View Email by ID\n";
            cout << "3. Delete Email by ID\n";
            cout << "4. Search Email by Sender\n";
            cout << "5. Search Email by Title\n";
            cout << "0. Return to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    displayEmails();
                    break;
                case 2: {
                    int id;
                    cout << "Enter email ID to view: ";
                    cin >> id;
                    viewEmail(id);
                    break;
                }
                case 3: {
                    int id;
                    cout << "Enter email ID to delete: ";
                    cin >> id;
                    deleteEmail(id);
                    break;
                }
                case 4: {
                    cin.ignore();
                    string sender;
                    cout << "Enter sender to search: ";
                    getline(cin, sender);
                    searchBySender(sender);
                    break;
                }
                case 5: {
                    cin.ignore();
                    string title;
                    cout << "Enter title to search: ";
                    getline(cin, title);
                    searchByTitle(title);
                    break;
                }
                case 0:
                    cout << "Returning to main menu...\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
                    break;
            }
        } while (choice != 0);
    }
};

#endif