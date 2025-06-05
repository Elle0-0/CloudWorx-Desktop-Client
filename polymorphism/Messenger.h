#ifndef MESSENGER_H
#define MESSENGER_H

#include <iostream>
#include <string>

// Function overloading example for sending notifications
void notify(const std::string& user) {
    std::cout << "Notify user: " << user << std::endl;
}

void notify(const std::string& user, int priority) {
    std::cout << "Notify user: " << user << " with priority " << priority << std::endl;
}

#endif // MESSENGER_H
