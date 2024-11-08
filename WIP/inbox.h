#ifndef INBOX_H
#define INBOX_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <unordered_set>

using namespace std;

// Renamed to InboxEmail to avoid conflict with outbox Email
struct InboxEmail {
    string sender;
    string receiver;
    string subject;
    string content;
    bool isSpam;

    InboxEmail(const string &sender, const string &receiver, const string &subject, const string &content, bool isSpam)
        : sender(sender), receiver(receiver), subject(subject), content(content), isSpam(isSpam) {}
};

// Node for the doubly linked list
struct Node {
    InboxEmail email;
    Node *prev;
    Node *next;

    Node(const InboxEmail &e) : email(e), prev(nullptr), next(nullptr) {}
};

// Doubly Linked List for Inbox Management
class DoublyLinkedList {
private:
    Node *head;
    Node *tail;

    bool isSpam(const string &content, const unordered_set<string> &spamWords) const {
        for (const auto &word : spamWords) {
            if (content.find(word) != string::npos) {
                return true;
            }
        }
        return false;
    }

    Node *getNodeByPosition(int position) const {
        int currentIndex = 1;
        Node *current = head;
        while (current != nullptr) {
            if (currentIndex == position) {
                return current;
            }
            currentIndex++;
            current = current->next;
        }
        return nullptr;
    }

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    ~DoublyLinkedList() {
        Node *current = head;
        while (current != nullptr) {
            Node *nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    void addEmail(const string &sender, const string &receiver, const string &subject, const string &content, const unordered_set<string> &spamWords) {
        bool spamFlag = isSpam(content, spamWords);
        Node *newNode = new Node(InboxEmail(sender, receiver, subject, content, spamFlag));
        if (tail == nullptr) { // List is empty
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    void displayEmails(const string &receiver, bool includeSpam) const {
        if (head == nullptr) {
            cout << "Inbox is empty.\n";
            return;
        }

        Node *current = head;
        int position = 1;
        cout << "No\tSender\t\tSubject\n";
        while (current != nullptr) {
            const InboxEmail &email = current->email;
            if (email.receiver == receiver && (includeSpam || !email.isSpam)) {
                cout << position << "\t" << email.sender << "\t\t" << email.subject << "\n";
            }
            position++;
            current = current->next;
        }
    }

    void viewEmail(int position, const string &receiver) const {
        Node *node = getNodeByPosition(position);
        if (node == nullptr || node->email.receiver != receiver || node->email.isSpam) {
            cout << "Email not found or is marked as spam.\n";
            return;
        }

        const InboxEmail &email = node->email;
        cout << "Sender: " << email.sender << "\n";
        cout << "Receiver: " << email.receiver << "\n";
        cout << "Subject: " << email.subject << "\n";
        cout << "Content:\n" << email.content << "\n";
    }

    void deleteEmail(int position, const string &receiver) {
        Node *node = getNodeByPosition(position);
        if (node == nullptr || node->email.receiver != receiver) {
            cout << "Email not found.\n";
            return;
        }

        if (node->prev) {
            node->prev->next = node->next;
        } else {
            head = node->next;
        }
        if (node->next) {
            node->next->prev = node->prev;
        } else {
            tail = node->prev;
        }
        delete node;

        cout << "Email deleted successfully.\n";
    }

    void searchBySender(const string &sender) const {
        Node *current = head;
        bool found = false;
        while (current != nullptr) {
            if (current->email.sender == sender) {
                const InboxEmail &email = current->email;
                cout << "Sender: " << email.sender << ", Subject: " << email.subject << "\n";
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "No emails found from sender: " << sender << "\n";
        }
    }

    void searchByTitle(const string &title) const {
        Node *current = head;
        bool found = false;
        string lowerTitle = toLower(title);
        while (current != nullptr) {
            string lowerSubject = toLower(current->email.subject);
            if (lowerSubject.find(lowerTitle) != string::npos) {
                const InboxEmail &email = current->email;
                cout << "Sender: " << email.sender << ", Subject: " << email.subject << "\n";
                found = true;
            }
            current = current->next;
        }
        if (!found) {
            cout << "No emails found with title: " << title << "\n";
        }
    }

    void loadEmailsFromFile(const string &filename, const unordered_set<string> &spamWords) {
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
                addEmail(sender, receiver, subject, content, spamWords);
            }
        }
        file.close();
        cout << "Emails loaded successfully from " << filename << ".\n";
    }

    static string toLower(const string &str) {
        string lowerStr = str;
        transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        return lowerStr;
    }

    void inboxMenu(const string &receiver, const unordered_set<string> &spamWords) {
        int choice;
        do {
            cout << "\nInbox Management\n";
            cout << "1. View All Received Emails\n";
            cout << "2. View Filtered Email by Position (No Spam)\n";
            cout << "3. Delete Email by Position\n";
            cout << "4. Search Email by Sender\n";
            cout << "5. Search Email by Title\n";
            cout << "0. Return to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    displayEmails(receiver, true);
                    break;
                case 2: {
                    int position;
                    cout << "Enter position to view: ";
                    cin >> position;
                    viewEmail(position, receiver);
                    break;
                }
                case 3: {
                    int position;
                    cout << "Enter position to delete: ";
                    cin >> position;
                    deleteEmail(position, receiver);
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
