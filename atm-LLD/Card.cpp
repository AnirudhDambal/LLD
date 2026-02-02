#include "include/Card.h"

Card::Card(const std::string &cardNumber, const std::string &pin)
    : cardNumber(cardNumber), pin(pin) {}

const std::string &Card::getCardNumber() const { return cardNumber; }
const std::string &Card::getPin() const { return pin; }
void Card::setPin(const std::string &p) { pin = p; }
