#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <iostream>

class Message {
public:
    Message(const std::string& sender, const std::string& recipient)
        : sender(sender), recipient(recipient) {
        std::cout << "Message constructor\n";
    }

    // Pure virtual function
    virtual void send() const = 0;

    // Virtual function
    virtual void preview() const {
        std::cout << "Preview message from " << sender << " to " << recipient << "\n";
    }

    // Virtual destructor
    virtual ~Message() {
        std::cout << "Message destructor\n";
    }

    // Operator overloading to compare messages by sender and recipient
    bool operator==(const Message& other) const {
        return sender == other.sender && recipient == other.recipient;
    }

protected:
    std::string sender;
    std::string recipient;
};

#endif // MESSAGE_H
