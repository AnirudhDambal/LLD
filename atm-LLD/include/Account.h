#pragma once
#include <string>
#include <unordered_map>

class Card;

class Account {
public:
    Account(const std::string &accountNumber, double balance = 0.0);

    const std::string &getAccountNumber() const;
    double getBalance() const;
    void deposit(double amount);
    bool withdraw(double amount);

    void linkCard(Card *card);

private:
    std::string accountNumber;
    double balance;
    std::unordered_map<std::string, Card*> cards;
};
