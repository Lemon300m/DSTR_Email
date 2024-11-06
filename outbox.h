// outbox.h
#ifndef OUTBOX_H
#define OUTBOX_H

#include <string>

void loadUserData(const std::string& filename);
bool isSpam(const std::string& content, const std::string& spamFile);
void addEmail(const std::string& sender, const std::string& receiver, const std::string& title, const std::string& content, const std::string& spamFile);
void displayOutbox();
void sendFirstEmail();

#endif
