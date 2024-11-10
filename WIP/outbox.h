#ifndef OUTBOX_H
#define OUTBOX_H

#include <iostream>
#include <fstream>
#include <string>
#include "commonFunction.h"

using namespace std;

struct outboxEmail {
    string sender;
    string receiver;
    string subject;
    string content;
    string time;
};

struct EmailNode {
    outboxEmail email;
    EmailNode* next;

    EmailNode(const string& sndr, const string& rcvr, const string& subj, const string& cntnt, const string& tm = "")
        : email{sndr, rcvr, subj, cntnt, tm}, next(nullptr) {}
};

class Outbox {
private:
    string senderEmail; // Store the sender's email
    EmailNode* sentEmailsHead;
    EmailNode* draftsHead;

public:
    // Constructor that accepts sender email
    Outbox(const string& sender) : senderEmail(sender), sentEmailsHead(nullptr), draftsHead(nullptr) {}

    // Send Email
    void sendEmail(const string& receiver, const string& subject, const string& content, bool isDraft) {
        string time = isDraft ? "" : getCurrentTimeAsString();
        EmailNode* newEmail = new EmailNode(senderEmail, receiver, subject, content, time);

        if (isDraft) {
            newEmail->next = draftsHead;
            draftsHead = newEmail;
            saveDrafts();
        } else {
            newEmail->next = sentEmailsHead;
            sentEmailsHead = newEmail;
            saveEmails();
        }
    }

    // Display Outbox
    void displayOutbox() const {
        EmailNode* current = sentEmailsHead;
        int count = 0;
        while (current != nullptr) {
            if (current->email.sender == senderEmail) {
                cout << "Email " << ++count << ":\n";
                cout << "To: " << current->email.receiver << "\nSubject: " << current->email.subject << "\nContent: " << current->email.content << "\nTime: " << current->email.time << "\n\n";
            }
            current = current->next;
        }
        if (count == 0) {
            cout << "No sent emails found for " << senderEmail << "\n";
        }
    }

    // Modify and Send Drafts
    void modifyAndSendDrafts() {
        EmailNode* current = draftsHead;
        int count = 0;
        while (current != nullptr) {
            if (current->email.sender == senderEmail) {
                cout << "Draft " << ++count << ":\n";
                cout << "To: " << current->email.receiver << "\nSubject: " << current->email.subject << "\nContent: " << current->email.content << "\n";
            }
            current = current->next;
        }
        if (count == 0) {
            cout << "No drafts found for " << senderEmail << "\n";
            return;
        }

        int choice;
        cout << "Select draft to modify (1-" << count << "): ";
        cin >> choice;

        if (choice <= 0 || choice > count) {
            cout << "Invalid choice.\n";
            return;
        }

        EmailNode* selectedDraft = draftsHead;
        EmailNode* prev = nullptr;
        for (int i = 1; i < choice; i++) {
            prev = selectedDraft;
            selectedDraft = selectedDraft->next;
        }

        int action;
        cout << "1. Send\n2. Delete\nChoose action: ";
        cin >> action;

        if (action == 1) {  // Send draft
            selectedDraft->email.time = getCurrentTimeAsString();
            removeDraft(selectedDraft, prev);
            selectedDraft->next = sentEmailsHead;
            sentEmailsHead = selectedDraft;
            saveEmails();
            cout << "Draft sent.\n";
        } else if (action == 2) {  // Delete draft
            removeDraft(selectedDraft, prev);
            delete selectedDraft;
            cout << "Draft deleted.\n";
        } else {
            cout << "Invalid action.\n";
        }
    }

    // Save sent emails to Emails.txt
    void saveEmails() const {
        ofstream outFile("Emails.txt");
        EmailNode* current = sentEmailsHead;
        while (current != nullptr) {
            outFile << current->email.sender << "," << current->email.receiver << "," << current->email.subject << "," << current->email.content << "," << current->email.time << "\n";
            current = current->next;
        }
        outFile.close();
    }

    // Save drafts to Drafts.txt
    void saveDrafts() const {
        ofstream outFile("Drafts.txt");
        EmailNode* current = draftsHead;
        while (current != nullptr) {
            outFile << current->email.sender << "," << current->email.receiver << "," << current->email.subject << "," << current->email.content << "\n";
            current = current->next;
        }
        outFile.close();
    }

    // Remove draft from the linked list
    void removeDraft(EmailNode* draftNode, EmailNode* prevNode) {
        if (prevNode == nullptr) {
            draftsHead = draftNode->next;
        } else {
            prevNode->next = draftNode->next;
        }
    }

    // Destructor to free linked list memory
    ~Outbox() {
        clearList(sentEmailsHead);
        clearList(draftsHead);
    }

private:
    void clearList(EmailNode* head) {
        while (head != nullptr) {
            EmailNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

// Outbox Menu
void outboxMenu(Outbox& outbox) {
    int choice;
    do {
        cout << "\n1. Send Email\n2. Display Outbox\n3. Modify and Send Drafts\n4. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string receiver, subject, content;
                int isDraftOption;
                cout << "Enter receiver email: ";
                cin >> receiver;
                cout << "Enter email subject: ";
                cin.ignore();
                getline(cin, subject);
                cout << "Enter email content: ";
                getline(cin, content);
                cout << "Save as Draft? (1 for Yes, 0 for No): ";
                cin >> isDraftOption;
                outbox.sendEmail(receiver, subject, content, isDraftOption);
                cout << (isDraftOption ? "Draft saved.\n" : "Email sent.\n");
                break;
            }
            case 2:
                outbox.displayOutbox();
                break;
            case 3:
                outbox.modifyAndSendDrafts();
                break;
            case 4:
                cout << "Exiting outbox menu.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);
}

#endif // OUTBOX_H
