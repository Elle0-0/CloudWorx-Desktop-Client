#ifndef TEXTMESSAGE_H
#define TEXTMESSAGE_H

#include "Message.h"
#include <iostream>

class TextMessage : public Message {
public:
    TextMessage(const std::string& sender, const std::string& recipient, const std::string& text)
        : Message(sender, recipient), text(text) {
        std::cout << "TextMessage constructor\n";
    }

    void send() const override {
        std::cout << "Sending text message: " << text << " from " << sender << " to " << recipient << "\n";
    }

    void preview() const override {
        std::cout << "Text preview: " << text.substr(0, 20) << "...\n";
    }

    ~TextMessage() override {
        std::cout << "TextMessage destructor\n";
    }

private:
    std::string text;
};

#endif // TEXTMESSAGE_H
