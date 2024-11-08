#ifndef EMAIL_MANAGEMENT_H
#define EMAIL_MANAGEMENT_H

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Struct to represent an email
struct Email {
    string sender;
    string receiver;
    string title;
    string content;
    Email* next;

    Email(const string& s, const string& r, const string& t, const string& c)
        : sender(s), receiver(r), title(t), content(c), next(nullptr) {}
};

// Queue class to manage emails in the outbox
class Queue {
    Email* front;
    Email* rear;

public:
    Queue() : front(nullptr), rear(nullptr) {}

    void enqueue(const string& sender, const string& receiver, const string& title, const string& content) {
        Email* newEmail = new Email(sender, receiver, title, content);
        if (rear == nullptr) {
            front = rear = newEmail;
        } else {
            rear->next = newEmail;
            rear = newEmail;
        }
    }

    void dequeue() {
        if (front == nullptr) {
            cout << "Outbox is empty." << endl;
            return;
        }
        Email* temp = front;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
    }

    void sendEmail() {
        if (front == nullptr) {
            cout << "No emails to send. Outbox is empty." << endl;
            return;
        }
        cout << "Sending Email:\n";
        cout << "Sender: " << front->sender 
                  << ", Receiver: " << front->receiver
                  << ", Title: " << front->title 
                  << ", Content: " << front->content << "\n";
        dequeue();
    }

    void displayOutbox() const {
        Email* current = front;
        if (current == nullptr) {
            cout << "Outbox is empty." << endl;
            return;
        }
        while (current) {
            cout << "Sender: " << current->sender 
                  << ", Receiver: " << current->receiver
                  << ", Title: " << current->title 
                  << ", Content: " << current->content << endl;
            current = current->next;
        }
    }

    ~Queue() {
        while (front) {
            dequeue();
        }
    }

    void saveToFile(const string& filename) const {
        ofstream file(filename, ios::app);
        Email* current = front;
        while (current) {
            file << current->sender << "," 
                 << current->receiver << "," 
                 << current->title << "," 
                 << current->content << "\n";
            current = current->next;
        }
        file.close();
    }
};

void loadUserData(const string& filename) {
    ifstream file(filename);
    string email, passwd, username;
    while (file >> email >> passwd >> username) {
        cout << "Email: " << email << ", Username: " << username << endl;
    }
    file.close();
}

bool isSpam(const string& content, const string& spamFile) {
    ifstream file(spamFile);
    string spamWord;
    while (file >> spamWord) {
        if (content.find(spamWord) != string::npos) {
            return true;
        }
    }
    return false;
}

void addEmail(Queue& outbox, const string& sender, const string& receiver, 
              const string& title, const string& content, const string& spamFile) {
    if (isSpam(content, spamFile)) {
        cout << "Warning: This email contains spam words and may not be sent." << endl;
    }
    outbox.enqueue(sender, receiver, title, content);
    outbox.saveToFile("emails.txt");
}

void displayMenu(Queue& outbox) {
    string sender, receiver, title, content, spamFile = "spam_words.txt";
    int choice;

    loadUserData("user.txt");

    do {
        cout << "\nEmail Management System\n";
        cout << "1. Add Email\n";
        cout << "2. Display Outbox\n";
        cout << "3. Send Email\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "Enter sender email: ";
                getline(cin, sender);
                cout << "Enter receiver email: ";
                getline(cin, receiver);
                cout << "Enter title: ";
                getline(cin, title);
                cout << "Enter content: ";
                getline(cin, content);

                addEmail(outbox, sender, receiver, title, content, spamFile);
                cout << "Email added to outbox.\n";
                break;

            case 2:
                cout << "\nOutbox contents:\n";
                outbox.displayOutbox();
                break;

            case 3:
                cout << "\nSending email:\n";
                outbox.sendEmail();
                break;

            case 4:
                cout << "Exiting program.\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);
}

#endif // EMAIL_MANAGEMENT_H
