#include "commonFunction.h"
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void wait(int milliseconds) {
    #ifdef _WIN32
        system(("timeout /T " + std::to_string(milliseconds / 1000) + " > nul").c_str());
    #else
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    #endif
}


string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    return (first == string::npos) ? "" : str.substr(first, (last - first + 1));
}

string getCurrentTimeAsString() {
    time_t currentTime = std::time(nullptr);
    tm* localTime = std::localtime(&currentTime);

    // Array of month abbreviations
    const string months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    stringstream timeStream;
    timeStream << setw(2) << setfill('0') << localTime->tm_mday << " "    // Day
               << months[localTime->tm_mon] << " "                         // Month abbreviation
               << (1900 + localTime->tm_year) << ", "                      // Year
               << setw(2) << setfill('0') << localTime->tm_hour << ":"     // Hour
               << setw(2) << setfill('0') << localTime->tm_min << "."      // Minute
               << setw(2) << setfill('0') << localTime->tm_sec;            // Second

    return timeStream.str();
}