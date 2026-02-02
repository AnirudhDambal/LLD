#include "include/Account.h"
#include "include/Card.h"

Account::Account(const std::string &accountNumber, double balance)
    : accountNumber(accountNumber), balance(balance) {}

const std::string &Account::getAccountNumber() const { return accountNumber; }
double Account::getBalance() const { return balance; }

void Account::deposit(double amount) { balance += amount; }

bool Account::withdraw(double amount) {
    if (balance >= amount) {
        balance -= amount;
        return true;
    }
    return false;
}

void Account::linkCard(Card *card) {
    if (!card) return;
    cards[card->getCardNumber()] = card;
}
