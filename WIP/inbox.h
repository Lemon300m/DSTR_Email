#ifndef INBOX_H
#define INBOX_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

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
    Node *spamHead;  // Head pointer for spam list
    Node *spamTail;
    string spamSymbols = "!#$%^&*";
    string spamWords[100];  // Array to store spam words
    int spamWordCount = 0;  // Counter for loaded spam words

    void loadSpamWords(const string &filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Failed to open the spam words file.\n";
            return;
        }

        string word;
        while (file >> word && spamWordCount < 100) {  // Limit to 100 words
            spamWords[spamWordCount++] = word;
        }
        file.close();
    }

    bool isSpam(const string &subject, const string &content) const {
    for (int i = 0; i < spamWordCount; ++i) {
        if (subject.find(spamWords[i]) != string::npos || content.find(spamWords[i]) != string::npos) {
            return true;
        }
    }
    return false;
    }


    bool containsSpamWords(const string &text) const {
        // Checks if any spam word is present in the given text
        for (int i = 0; i < spamWordCount; ++i) {
            if (text.find(spamWords[i]) != string::npos) {
                return true;
            }
        }
        return false;
    }

    Node *getNodeByPosition(int position ,bool isSpamList) const {
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
    DoublyLinkedList() : head(nullptr), tail(nullptr),spamHead(nullptr), spamTail(nullptr) {
        loadSpamWords("spam_words.txt");  // Load spam words on initialization
    }

    ~DoublyLinkedList() {
        Node *current = head;
        while (current != nullptr) {
            Node *nextNode = current->next;
            delete current;
            current = nextNode;
        }

        current = spamHead;
        while (current != nullptr) {
            Node *nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    void addEmail(const string &sender, const string &receiver, const string &subject, const string &content) 
    {
        bool spamFlag = isSpam(subject, content);
        Node *newNode = new Node(InboxEmail(sender, receiver, subject, content, spamFlag));
        
        if (spamFlag) {
            // Add to spam list
            if (spamTail == nullptr) {
                spamHead = spamTail = newNode;
            } else {
                spamTail->next = newNode;
                newNode->prev = spamTail;
                spamTail = newNode;
            }
        } else {
            // Add to main inbox list
            if (tail == nullptr) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
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



    void displaySpamEmails() const {
        if (spamHead == nullptr) {
            cout << "Spam folder is empty.\n";
            return;
        }

        Node *current = spamHead;
        int position = 1;
        cout << "No\tSender\t\tSubject\n";
        while (current != nullptr) {
            const InboxEmail &email = current->email;
            cout << position << "\t" << email.sender << "\t\t" << email.subject << "\n";
            position++;
            current = current->next;
        }
    }



    void viewEmail(int position, const string &receiver, bool isSpamList) const {
        Node *node = getNodeByPosition(position,isSpamList);
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

    void deleteEmail(int position, const string &receiver,bool isSpamList) {
        Node *node = getNodeByPosition(position, isSpamList);
        if (node == nullptr || node->email.receiver != receiver) {
            cout << "Email not found.\n";
            return;
        }

        if (node->prev) {
            node->prev->next = node->next;
        } else {
            if (isSpamList) spamHead = node->next;
            else head = node->next;
        }
        if (node->next) {
            node->next->prev = node->prev;
        } else {
            if (isSpamList) spamTail = node->prev;
            else tail = node->prev;
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

    void loadEmailsFromFile(const string &filename) {
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
                addEmail(sender, receiver, subject, content);
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

    void inboxMenu(const string &receiver ,string spamWords[], int spamWordCount) {
        int choice;
        do {
            cout << "\nInbox Management\n";
            cout << "1. View All Received Emails\n";
            cout << "2. View Spam Emails\n";
            cout << "3. View Filtered Email by Position (No Spam)\n";
            cout << "4. Delete Email by Position\n";
            cout << "5. Delete Spam Email by Position\n";
            cout << "6. Search Email by Sender\n";
            cout << "7. Search Email by Title\n";
            cout << "0. Return to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    displayEmails(receiver, false);
                    break;
                case 2:
                    displaySpamEmails();  // Show only spam emails
                    break;
                case 3: {
                    int position;
                    cout << "Enter position to view: ";
                    cin >> position;
                    viewEmail(position, receiver,false);
                    break;
                }
                case 4: {
                    int position;
                    cout << "Enter position to delete: ";
                    cin >> position;
                    deleteEmail(position, receiver,false);
                    break;
                }
                case 5: {
                int position;
                cout << "Enter position to delete from spam: ";
                cin >> position;
                deleteEmail(position, receiver, true);
                break;
                }
                case 6: {
                    cin.ignore();
                    string sender;
                    cout << "Enter sender to search: ";
                    getline(cin, sender);
                        (sender);
                    break;
                }
                case 7: {
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
