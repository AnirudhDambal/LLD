#pragma once
#include <string>

class Card {
public:
    Card(const std::string &cardNumber, const std::string &pin);
    const std::string &getCardNumber() const;
    const std::string &getPin() const;
    void setPin(const std::string &pin);

private:
    std::string cardNumber;
    std::string pin;
};
