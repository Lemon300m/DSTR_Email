#include "outbox.h"
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

// Create a static Queue instance to manage emails within this file only
static Queue outbox;

// Load user data from a file
void loadUserData(const string& filename) {
    ifstream file(filename);
    string email, passwd, username;
    while (file >> email >> passwd >> username) {
        cout << "Email: " << email << ", Username: " << username << endl;
    }
    file.close();
}

// Check for spam words in email content
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

// Add an email directly to the internal queue without requiring a Queue parameter
void addEmail(const string& sender, const string& receiver, const string& title, const string& content, const string& spamFile) {
    if (isSpam(content, spamFile)) {
        cout << "Warning: This email contains spam words and may not be sent." << endl;
    }
    outbox.enqueue(sender, receiver, title, content);
    outbox.saveToFile("emails.txt");  // Save to file after adding
}

void displayOutbox() {
    outbox.displayOutbox();
}

void sendFirstEmail() {
    outbox.sendEmail();
}
