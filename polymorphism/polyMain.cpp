// #include <iostream>
// #include <memory>
// #include "Message.h"
// #include "TextMessage.h"
// #include "ImageMessage.h"
// #include "Messenger.h"

// int main() {
//     // Compile-time polymorphism: overloaded notify functions
//     notify("alice@example.com");
//     notify("bob@example.com", 2);

//     // Create messages using base class pointers
//     std::unique_ptr<Message> msg1 = std::make_unique<TextMessage>("alice@example.com", "bob@example.com", "Hello, Bob!");
//     std::unique_ptr<Message> msg2 = std::make_unique<ImageMessage>("carol@example.com", "dave@example.com", "/images/pic.png");

//     // Runtime polymorphism: virtual send()
//     msg1->send();
//     msg2->send();

//     // Virtual preview()
//     msg1->preview();
//     msg2->preview();

//     // Operator overloading: compare senders and recipients
//     TextMessage tm1("alice@example.com", "bob@example.com", "Hi!");
//     TextMessage tm2("alice@example.com", "bob@example.com", "Hello!");

//     if (tm1 == tm2) {
//         std::cout << "Messages have the same sender and recipient\n";
//     } else {
//         std::cout << "Messages are different\n";
//     }

//     return 0;
// }
