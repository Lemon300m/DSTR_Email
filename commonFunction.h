#ifndef COMMONFUNCTION_H
#define COMMONFUNCTION_H

#include <string>
#include <iostream>
#include <thread>     // For cross-platform sleep functionality
#include <chrono>

void clearScreen();
void wait(int milliseconds);
std::string trim(const std::string& str);
std::string getCurrentTimeAsString();

#endif
