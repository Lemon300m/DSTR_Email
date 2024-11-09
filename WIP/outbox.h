#ifndef EMAIL_MANAGEMENT_H
#define EMAIL_MANAGEMENT_H

#include <iostream>
#include <fstream>
#include <string>
#include "commonFunction.h"

using namespace std;

struct Email {
    string sender;
    string receiver;
    string title;
    string content;
    string time; 
};

struct Node {
    Email email;
    Node* next;
    Node(const Email& emailData) : email(emailData), next(nullptr) {}
};

class EmailList {
private:
    Node* head;
    int count;

public:
    EmailList() : head(nullptr), count(0) {}

    ~EmailList() {
        clear();
    }

    void addEmail(const Email &email) {
        Node* newNode = new Node(email);
        if (!head) {
            head = newNode;
        } else {
            Node* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        count++;
    }

    int getCount() const { return count; }

    // Marking getEmail as const and returning const Email&
    const Email& getEmail(int index) const {
        if (index < 0 || index >= count) {
            throw out_of_range("Invalid index");
        }
        Node* temp = head;
        for (int i = 0; i < index; ++i) {
            temp = temp->next;
        }
        return temp->email;
    }

    void removeEmail(int index) {
        if (index < 0 || index >= count) {
            cout << "Invalid index\n";
            return;
        }
        Node* temp = head;
        if (index == 0) {
            head = head->next;
            delete temp;
        } else {
            Node* prev = nullptr;
            for (int i = 0; i < index; ++i) {
                prev = temp;
                temp = temp->next;
            }
            prev->next = temp->next;
            delete temp;
        }
        count--;
    }

    void displayEmails(const string& sender) const {
        Node* temp = head;
        while (temp) {
            if (temp->email.sender == sender) {
                cout << "Receiver: " << temp->email.receiver
                     << "\nTitle: " << temp->email.title
                     << "\nContent: " << temp->email.content
                     << (temp->email.time.empty() ? "" : "\nTime: " + temp->email.time)
                     << "\n\n";
            }
            temp = temp->next;
        }
    }

    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        count = 0;
    }
};

class Outbox {
private:
    EmailList sentEmails;
    EmailList drafts;
    string currentUserEmail;

public:
    Outbox(const string &email) : currentUserEmail(email) {}

    void sendEmail(const string& receiver, const string& title, const string& content, bool isDraft = false) {
        string time = getCurrentTimeAsString();
        Email email{currentUserEmail, receiver, title, content, isDraft ? "" : time};
        if (isDraft) {
            drafts.addEmail(email);
            saveDraftsToFile();
        } else {
            sentEmails.addEmail(email);
            saveEmailsToFile();
        }
    }

    void displayOutbox() const {
        sentEmails.displayEmails(currentUserEmail);
    }

    void modifyAndSendDraft() {
        drafts.displayEmails(currentUserEmail);

        int choice;
        cout << "Enter the draft number to send or delete (1 to " << drafts.getCount() << "): ";
        cin >> choice;
        choice -= 1; // Adjust for array index

        if (choice >= 0 && choice < drafts.getCount()) {
            const Email& draft = drafts.getEmail(choice);
            cout << "1. Send Draft\n2. Delete Draft\nChoose an option: ";
            int option;
            cin >> option;

            if (option == 1) {
                Email modifiedDraft = draft;
                string time = getCurrentTimeAsString();
                modifiedDraft.time = time;
                sentEmails.addEmail(modifiedDraft);
                drafts.removeEmail(choice);
                saveEmailsToFile();
                saveDraftsToFile();
                cout << "Draft sent successfully.\n";
            } else if (option == 2) {
                drafts.removeEmail(choice);
                saveDraftsToFile();
                cout << "Draft deleted successfully.\n";
            } else {
                cout << "Invalid option.\n";
            }
        } else {
            cout << "Invalid draft number.\n";
        }
    }

    void saveEmailsToFile() const {
        ofstream outFile("Emails.txt", ios::trunc);
        for (int i = 0; i < sentEmails.getCount(); ++i) {
            const Email& email = sentEmails.getEmail(i);
            outFile << email.sender << "," << email.receiver << "," << email.title << "," << email.content << "," << email.time << "\n";
        }
    }

    void saveDraftsToFile() const {
        ofstream outFile("draft.txt", ios::trunc);
        for (int i = 0; i < drafts.getCount(); ++i) {
            const Email& email = drafts.getEmail(i);
            outFile << email.sender << "," << email.receiver << "," << email.title << "," << email.content << "\n";
        }
    }
};

int main() {
    string currentUserEmail;
    cout << "Enter your email: ";
    cin >> currentUserEmail;

    Outbox outbox(currentUserEmail);

    int choice;
    do {
        cout << "\n1. Send Email\n2. Display Outbox\n3. Modify and Send Drafts\n4. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string receiver, title, content;
                int isDraftOption;
                cout << "Enter receiver email: ";
                cin >> receiver;
                cout << "Enter email title: ";
                cin.ignore();
                getline(cin, title);
                cout << "Enter email content: ";
                getline(cin, content);
                cout << "Save as Draft? (1 for Yes, 0 for No): ";
                cin >> isDraftOption;
                outbox.sendEmail(receiver, title, content, isDraftOption);
                cout << (isDraftOption ? "Draft saved.\n" : "Email sent.\n");
                break;
            }
            case 2:
                outbox.displayOutbox();
                break;

            case 3:
                outbox.modifyAndSendDraft();
                break;

            case 4:
                cout << "Exiting program.\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    return 0;
}

#endif // EMAIL_MANAGEMENT_H
