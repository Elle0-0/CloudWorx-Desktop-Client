#ifndef IMAGEMESSAGE_H
#define IMAGEMESSAGE_H

#include "Message.h"
#include <iostream>

class ImageMessage : public Message {
public:
    ImageMessage(const std::string& sender, const std::string& recipient, const std::string& imagePath)
        : Message(sender, recipient), imagePath(imagePath) {
        std::cout << "ImageMessage constructor\n";
    }

    void send() const override {
        std::cout << "Sending image message: " << imagePath << " from " << sender << " to " << recipient << "\n";
    }

    void preview() const override {
        std::cout << "Image preview: " << imagePath << "\n";
    }

    ~ImageMessage() override {
        std::cout << "ImageMessage destructor\n";
    }

private:
    std::string imagePath;
};

#endif // IMAGEMESSAGE_H
